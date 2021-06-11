This file is meant to describe how to use http_client.hpp file.
The http_client class is a wrapper over libcurl to perform HTTP requests with ease
The file exports class http_client with member functions get, getfile, put, putfile, simplepost, binarypost and formpost, their customization counterparts and logging functionalities.

The first 6 functions are simple and intuitive to interpret using signatures. So are the logging functionalities.

The file also exports class mime_part and 2 specializations, mime_file_part and mime_string_part. These are used in creating a formpost. Create parts on the fly and add pointers to these (converted to mime_part*) to a vector and pass this vector to formpost(). For uploading multiple files under the same field name, pass multiple mime_part*s in the vector.

Each function also comes with a c_ counterpart which essentially changes the method of the request to a custom name provided by the user. Custom counterparts are provided to all functions to keep an exhaustive list. For example, a HEAD request can be made by making a c_get() with type = "HEAD".

When response is returned in a string, passing nullptr / not passing the argument will simply ignore the response.

==================================================================

Expected future improvements :
1. Use of multi interface so that calls are non blocking
