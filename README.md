# A Very Minimalistic C/C++ HTTP(S) Library Built With Raw Sockets and OpenSSL #


- Exposed Headers (because why use -lssl -lcrypto if you're not using https, the less code the better):

	- httpget.h 
		- HttpGet() - return `char* buffer`
	
	- httpsget.h 
		- HttpsGet() - return `char* buffer`
	
	- httppost.h 
		- HttpPost() - return `char* buffer`
	
	- httpspost.h 
		- HttpsPost() - return `char* buffer`



reason: i wanted to perform https requests in one of my projects and needed it to by static compiled as it is objectively better from a malware pov to NOT depend on the target system plus it was a POSIX kernel module and i didn't want a kernel module to break because of a dependency issue

- why not libcurl: it is nearly impossible to statically link libcurl without modifying and rebuilding core system libraries


So i made this *very* simple lib that does the job and doesn't make you link against a whole bunch of random libs you are not using

- function signatures:
```C
char* HttpGet(const char* http_server, const char* url_path, int* response_code)
char* HttpsGet(const char* https_server, const char* url_path, int* response_code)

char* HttpPost(const char* http_server, const char* url_path, const char* post_data, int* response_code)
char* HttpsPost(const char* https_server, const char* url_path, const char* post_data, int* response_code)
```

- Note: i made the functions to take the server alone and the url path alone because *i value my sanity :)* 


Usage is very simple:

```C
#include "httpget.h"
#include "httpsget.h"

#include "httppost.h"
#include "httpspost.h"


int main(){


    /* ---------------- GET HTTP ------------------------*/

    int response_code;

    char* response = HttpGet("example.com", "/", &response_code); 

    if (response_code == 200){
        printf("Server Returned: OK\n");
    }

   printf("%s\n", response); // prints the whole http response
    


    /* ---------------- GET HTTPS ------------------------*/

    int response_code;

    char* response = HttpsGet("example.com", "/", &response_code); 

    if (response_code == 200){
        printf("Server Returned: OK\n");
    }

    printf("\n\n%s\n", response);



    /* ---------------- POST HTTP ------------------------*/


    const char* post_data = "hello=world";
    int response_code;

    char* response = HttpPost("example.com", "/", post_data, &response_code);

    printf("%d\n", response_code);
    printf("\n\n%s\n", response);


    /* ---------------- POST HTTPS ------------------------*/


    const char *urlpath = "/botXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/sendMessage";
    const char *postdata = "chat_id=-133713371337&text=HelloWorld from libhttpmin"; 
    int response_code;


    char* response = HttpsPost("api.telegram.org", urlpath, postdata, &response_code);

    if (response_code == 200){
        fprintf(stdout, "Sent Message\n");
    }

    printf("%s\n", response);




    return 0;
}
```
- compiling:

	- without ssl (httpget/httppost): `gcc prog.c -o prog` for static linking: `gcc prog.c -o prog -static`

	- with ssl (httpsget/httpspost): `gcc prog.c -o prog -lssl -lcrypto` for static linking: `gcc prog.c -o prog -static -lssl -lcrypto -lzstd -l:libz.a` 


**No need to include all 4 headers, use only what your program will use**



