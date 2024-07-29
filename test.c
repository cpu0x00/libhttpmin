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
