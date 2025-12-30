#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

static char* conv_pass;

int (*pam_show_cb)(const char* msg) = puts;

static int conversation(int num_msg, const struct pam_message **msg,
                        struct pam_response **resp, void *appdata_ptr) {
    // Allocate memory for responses
    *resp = (struct pam_response *)malloc(num_msg * sizeof(struct pam_response));
    if (*resp == NULL) {
        return PAM_BUF_ERR; // Memory allocation error
    }

    for (int i = 0; i < num_msg; i++) {
        pam_show_cb(msg[i]->msg);
        puts(msg[i]->msg);
        (*resp)[i].resp = strdup(conv_pass); // Replace with dynamic input if needed
        (*resp)[i].resp_retcode = 0; // Set return code
    }
    return PAM_SUCCESS; // Indicate success
}


static struct pam_conv conv= {
    conversation,
    NULL
};;

bool pam_auth(const char *username, const char *password) {


    pam_handle_t *pamh = NULL;
    int retval;

    // Start PAM transaction
    retval = pam_start("su", username, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
        fprintf(stderr, "pam_start failed: %s\n", pam_strerror(pamh, retval));
        return false;
    }

    // Set the password for authentication
    conv_pass = strdup(password);

    // Authenticate the user
    retval = pam_authenticate(pamh, 0);
    if (retval == PAM_SUCCESS) {
        printf("Authentication successful!\n");
    } else {
        fprintf(stderr, "Authentication failed: %s\n", pam_strerror(pamh, retval));
        pam_show_cb(pam_strerror(pamh, retval));
    }

    // End PAM transaction
    pam_end(pamh, retval);
    free(conv_pass);
    return (retval == PAM_SUCCESS);
}

