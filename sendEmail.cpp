#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>   
#include <ctype.h>

#define MAX_LEN 512
#define BODY_FILE "email_body.txt"
#define PS1_FILE  "send_email_temp.ps1"

char userEmail[MAX_LEN];
char userPass[128];
char recipient[MAX_LEN];
char subjectLine[MAX_LEN];


void readPassword(char *buf, size_t bufsz) {
    size_t idx = 0;
    int ch;
    printf("App password (input hidden): ");
    while ((ch = getch()) != '\r' && ch != '\n') {
        if (ch == 3) { /* Ctrl-C */
            exit(1);
        } else if (ch == 8) { /* backspace */
            if (idx > 0) {
                idx--;
                printf("\b \b");
            }
        } else if (idx + 1 < bufsz) {
            buf[idx++] = (char)ch;
            /* show asterisk to indicate a character was typed */
            printf("*");
        }
    }
    buf[idx] = '\0';
    printf("\n");
}

/* Escape single quotes in a string for safe single-quoted PowerShell usage:
   Replaces every single quote ' with '\'' style (two single quotes inside single-quoted PS string).
   For simplicity we double single-quotes (PowerShell treats '' as literal ' inside single-quoted string). */
void escapeSingleQuotes(const char *src, char *dst, size_t dstsz) {
    size_t i=0, j=0;
    while (src[i] != '\0' && j + 1 < dstsz) {
        if (src[i] == '\'') {
            if (j + 2 >= dstsz) break;
            dst[j++] = '\'';
            dst[j++] = '\'';
            i++;
        } else {
            dst[j++] = src[i++];
        }
    }
    dst[j] = '\0';
}

/* Get login (typed every run) */
void login() {
    printf("=== Login (type your university Gmail and App Password) ===\n");
    printf("Gmail address: ");
    if (!fgets(userEmail, sizeof(userEmail), stdin)) exit(1);
    /* remove newline */
    userEmail[strcspn(userEmail, "\r\n")] = '\0';

    readPassword(userPass, sizeof(userPass));
}

/* Compose message and save body to BODY_FILE */
void composeMessage() {
    FILE *f;
    char line[1024];

    printf("=== Compose Message ===\n");
    printf("Recipient email: ");
    if (!fgets(recipient, sizeof(recipient), stdin)) exit(1);
    recipient[strcspn(recipient, "\r\n")] = '\0';

    printf("Subject: ");
    if (!fgets(subjectLine, sizeof(subjectLine), stdin)) exit(1);
    subjectLine[strcspn(subjectLine, "\r\n")] = '\0';

    printf("Enter message body. End with a single dot (.) on a line by itself.\n");

    f = fopen(BODY_FILE, "w");
    if (!f) {
        perror("Unable to create body file");
        return;
    }

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;
        /* if line is ".\n" or ".\r\n" or "." */
        if ((line[0] == '.' && (line[1] == '\n' || line[1] == '\r' || line[1] == '\0')) ||
            (line[0] == '.' && line[1] == '\r' && line[2] == '\n')) {
            break;
        }
        fputs(line, f);
    }
    fclose(f);
    printf("Message saved to %s\n", BODY_FILE);
}

/* Build a PowerShell script file and call it to send the email */
void sendEmail() {
    FILE *ps;
    char escapedFrom[MAX_LEN * 2];
    char escapedTo[MAX_LEN * 2];
    char escapedSubject[MAX_LEN * 2];
    char command[1024];
    int rc;

    /* Basic checks */
    if (userEmail[0] == '\0' || userPass[0] == '\0') {
        printf("You must login first (menu option 1).\n");
        return;
    }
    if (recipient[0] == '\0') {
        printf("You must compose a message first (menu option 2).\n");
        return;
    }

    /* Escape single quotes for safe single-quoted PS literals */
    escapeSingleQuotes(userEmail, escapedFrom, sizeof(escapedFrom));
    escapeSingleQuotes(recipient, escapedTo, sizeof(escapedTo));
    escapeSingleQuotes(subjectLine, escapedSubject, sizeof(escapedSubject));

    /* Create the PowerShell script */
    ps = fopen(PS1_FILE, "w");
    if (!ps) {
        perror("Unable to create PowerShell script");
        return;
    }

    /* PowerShell script content:
       - Read body with Get-Content -Raw
       - Convert password to SecureString
       - Create PSCredential
       - Send-MailMessage using smtp.gmail.com:587 with SSL
    */
    fprintf(ps,
        "$From = '%s'\n"
        "$To = '%s'\n"
        "$Subject = '%s'\n"
        "$Body = Get-Content -Raw '%s'\n"
        "$Plain = '%s'\n"
        "$SecurePass = ConvertTo-SecureString $Plain -AsPlainText -Force\n"
        "$Cred = New-Object System.Management.Automation.PSCredential($From, $SecurePass)\n"
        "Send-MailMessage -From $From -To $To -Subject $Subject -Body $Body -SmtpServer 'smtp.gmail.com' -Port 587 -UseSsl -Credential $Cred\n",
        escapedFrom, escapedTo, escapedSubject, BODY_FILE, userPass
    );

    fclose(ps);

    /* Run PowerShell script: ExecutionPolicy Bypass to allow running locally created script */
    snprintf(command, sizeof(command),
             "powershell -ExecutionPolicy Bypass -File \"%s\"", PS1_FILE);

    printf("Sending email... (PowerShell will run)\n");
    rc = system(command);

    if (rc == 0) {
        printf("PowerShell returned 0 — attempt completed. If there are no errors shown above, the message likely was sent.\n");
    } else {
        printf("PowerShell returned code %d. There may have been an error. Check your credentials, app password, and whether your admin allows App Passwords.\n", rc);
    }

    /* cleanup temp files */
    if (remove(PS1_FILE) == 0) {
        /* removed */
    }
    if (remove(BODY_FILE) == 0) {
        /* removed */
    }

    /* For security, clear the password string in memory */
    memset(userPass, 0, sizeof(userPass));
}

/* Simple main menu using void functions */
void mainMenu() {
    int choice = 0;
    char buff[16];

    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1) Login (type your email and app password)\n");
        printf("2) Compose Message (recipient, subject, body)\n");
        printf("3) Send Email\n");
        printf("4) Exit\n");
        printf("Choice: ");
        if (!fgets(buff, sizeof(buff), stdin)) break;
        choice = atoi(buff);

        switch (choice) {
            case 1: login(); break;
            case 2: composeMessage(); break;
            case 3: sendEmail(); break;
            case 4:
                printf("Exiting. Goodbye!\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

int main(void) {
    /* init */
    userEmail[0] = userPass[0] = recipient[0] = subjectLine[0] = '\0';
    printf("Simple C Email Sender (Windows + Gmail backend)\n");
    printf("Note: you must use a Gmail App Password (or university app password) — regular account password will be rejected.\n");
    mainMenu();
    return 0;
}
