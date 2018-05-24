#include "WebPages.h"



const char PROGMEM INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>MiLight router network configuration</title>"
"</head>"
"<body>"
"<h1>MiLight router network configuration</h1>"
"<FORM action=\"/\" method=\"post\">"
"<P>"
"LED<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"1\">On<BR>"
"<INPUT type=\"radio\" name=\"LED\" value=\"0\">Off<BR>"
"<INPUT type=\"submit\" value=\"Send\"> <INPUT type=\"reset\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

namespace WebPages
{
    void ConfigRoot()
    {
        std::string site(INDEX_HTML);
        webServer.send(200, "text/html", site.c_str());
    }
    
}
