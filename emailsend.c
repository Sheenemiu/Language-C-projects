#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>

#define FROM_ADDR "insert your email here.com"


char TO_ADDR[100];  
char subject[256];
char body[1024];
char payload_buffer[1500];  

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    const char **payload = (const char **) userp;
    size_t buffer_size = size * nmemb;

    if (*payload == NULL)
        return 0;

    size_t len = strlen(*payload);
    if (len < buffer_size) {
        memcpy(ptr, *payload, len);
        *payload = NULL;
        return len;
    } else {
        memcpy(ptr, *payload, buffer_size);
        *payload += buffer_size;
        return buffer_size;
    }
}

int send_email(const char *subject, const char *body)
{
    CURL *curl;
    CURLcode res = CURLE_OK;

    snprintf(payload_buffer, sizeof(payload_buffer),
        "To: %s\r\n"
        "From: %s\r\n"
        "Subject: %s\r\n"
        "\r\n"
        "%s\r\n",
        TO_ADDR, FROM_ADDR, subject, body
    );

    const char *payload_text = payload_buffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        curl_easy_setopt(curl, CURLOPT_USERNAME, FROM_ADDR);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "fktz bczq phzd iqct ");  // ← change this

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_ADDR);

        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, TO_ADDR);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &payload_text);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "Email send failed: %s\n", curl_easy_strerror(res));
        else
            printf("\nEmail sent successfully!\n");

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
    return res;
}

int main()
{
    char choice;

    printf("Enter email address to send: ");
    scanf("%99s", TO_ADDR); 
    clear_input_buffer();  

    printf("\n========================================\n");
    printf("                SMAIL\n");
    printf("========================================\n");

    printf("From: %s\n", FROM_ADDR);
    printf("To:   %s\n", TO_ADDR);
    
    printf("========================================\n\n");

    do {
        printf("\n\n");
        printf("Compose your email:\n");
        printf("\n");

        printf("\nEnter subject: ");
        fgets(subject, sizeof(subject), stdin);
        subject[strcspn(subject, "\n")] = 0; 

        printf("\nEnter message body:\n");
        printf("(Type your message and press ENTER when you are done)\n");
        printf("> ");
        fgets(body, sizeof(body), stdin);
        body[strcspn(body, "\n")] = 0;  

        printf("\n========================================\n");
        printf("Email Preview:\n");
        printf("========================================\n");
        printf("Subject: %s\n", subject);
        printf("Message: %s\n", body);
        printf("========================================\n");

        printf("\nSend this email? (y/n): ");
        scanf(" %c", &choice);
        clear_input_buffer();  
        if (choice == 'y' || choice == 'Y') {
            int result = send_email(subject, body);
            if (result != 0) {
                printf("\nerror\n");
            }
        } else {
            printf("\nEmail cancelled.\n");
        }
        printf("\nSend another email? (y/n): ");
        scanf(" %c", &choice);
        clear_input_buffer();  
    } while (choice == 'y' || choice == 'Y');
    return 0;
}
