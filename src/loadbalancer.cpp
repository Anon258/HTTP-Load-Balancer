#include "loadbalancer.hpp"

void loadbalancer::health_check()
{
	start = std::time(nullptr);
	while(1)
	{
		curr = std::time(nullptr);
		if(curr - start >= interval)
		{	
			for(auto &sv: servers)
				send_health(sv);
			start = curr;
				
		}
		
	}
}

void loadbalancer::send_health(std::tuple<std::string, bool, std::string> &sv)
{	
	if(std::get<2>(sv) != "")
	{
		minimal_httpclient internal;
		header_map hds, res_hds;
		std::string response;
		long rescode;
		if (internal.request(std::get<0>(sv), "GET", "Regular Health Checkup", &response, hds, &res_hds, rescode) == CURLE_OK)
		{
			std::cout << "Health Check Successful for server " << std::get<0>(sv) << std::endl;
			std::get<1>(sv) = true;
		}
		else
		{
			std::cout << "Health Check Failed for server " << std::get<0>(sv) << std::endl;
			std::get<1>(sv) = false;
		}
	}
}

std::shared_ptr<http_response> loadbalancer::send_req(const http_request &req, std::string server)
{
	std::map<std::string, std::string, http::header_comparator> headers = req.get_headers();
	header_map hds;
	for (auto h : headers)
	{
	    hds.insert(h);
	}
	std::string method = req.get_method();
	std::string body = req.get_content();
	header_map res_hds;
	std::string response;
	long rescode;

	minimal_httpclient internal;
	if (internal.request(std::get<0>(servers[0]), method, body, &response, hds, &res_hds, rescode) == CURLE_OPERATION_TIMEDOUT)
	{
		timeouts++;
		std::cout << "Request with ID " << nreq << " timed out" << std::endl;
	}
	else
	{
		nserved++;
		std::cout << "Serving request with ID " << nreq << " now" << std::endl;
	}

	std::shared_ptr<http_response> res = std::shared_ptr<http_response>(new string_response(response, (int)rescode));
	for (auto rh : res_hds)
	{
		res->with_header(rh.first, rh.second);
	}

	return res;	
}

std::string loadbalancer::round_robin_select()
{
	for(int i = 0; i < nservers; i++)
	{
		if(std::get<1>(servers[(next_serv + i) % nservers]))
		{
			next_serv = (next_serv + i + 1) % nservers;
			return std::get<0>(servers[(next_serv + i) % nservers]);
		}
			
	}
	return "";
}

const std::shared_ptr<http_response> loadbalancer::render(const http_request &req)
{
	nreq++;
	std::cout << "Started processing request number " << nreq << std::endl;

	std::string server = round_robin_select();
	std::cout << "Server chosen: " << server << std::endl;
	if(server == "")
		return std::shared_ptr<http_response>(new string_response("Server is unreachable", 500));
	return send_req(req, server);
}
