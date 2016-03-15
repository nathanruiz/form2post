# form2post #
_form2post_ was created to automate scraping web pages for HTML form and input
tags in order to get the URL that can be used to submit it. This makes jobs such
as logging into a web site with curl much easier, as you don't even need to use
an XML/HTML parser.

I've attempted to follow the UNIX philosophy of writing small utilities that
only do one, unambiguous task. Therefore, this tool doesn't connect over the
network to a web server, but instead takes a web page from STDIN, and writes the
resulting GET/POST request to STDOUT.

## Examples ##

    $ curl http://example.com/ | form2post

    $ wget http://example.com/ -O - | form2post

    $ cat index.html | form2post
