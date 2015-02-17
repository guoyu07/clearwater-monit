/*
 * Copyright (C) Tildeslash Ltd. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 *
 * You must obey the GNU Affero General Public License in all respects
 * for all of the code used other than OpenSSL.
 */

#ifndef SSL_H
#define SSL_H

#include "config.h"

#ifdef HAVE_OPENSSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef OPENSSL_FIPS
#include <openssl/fips.h>
#endif
#endif


typedef enum {
        SSL_Auto = 0,
        SSL_V2,
        SSL_V3,
        SSL_TLSV1,
        SSL_TLSV11,
        SSL_TLSV12,
        SSL_Disabled
} __attribute__((__packed__)) SSL_Version;


/** Defines an SSL object */
typedef struct myssl {
        boolean_t use_ssl;             /**< true if SSL is required for connection */
        SSL_Version version;            /**< The SSL version to use for connection */
        char *certmd5;       /**< The expected md5 sum of the server's certificate */
        char *clientpemfile;                      /**< Optional client certificate */
} Ssl_T;


#ifdef HAVE_OPENSSL


typedef struct my_ssl_connection {
        int               socket;
        boolean_t         accepted;
        SSL              *handler;
        SSL_CTX          *ctx;
        X509             *cert;
        const SSL_METHOD *method;
        BIO              *socket_bio;
        const char       *cipher;
        char             *cert_subject;
        char             *cert_issuer;
        unsigned char    *cert_md5;
        unsigned int      cert_md5_len;
        char             *clientpemfile;

        struct my_ssl_connection *prev;
        struct my_ssl_connection *next;
} ssl_connection;


typedef struct my_ssl_server_connection {
        int               server_socket;
        const SSL_METHOD *method;
        SSL_CTX          *ctx;
        char             *pemfile;
        char             *clientpemfile;
        ssl_connection   *ssl_conn_list;
} ssl_server_connection;


#define                have_ssl() 1
void                   stop_ssl();
boolean_t              embed_ssl_socket(ssl_connection *, int);
boolean_t              embed_accepted_ssl_socket(ssl_connection *, int);
boolean_t              close_ssl_socket(ssl_connection *);
void                   close_accepted_ssl_socket(ssl_server_connection *, ssl_connection *);
void                   delete_ssl_socket(ssl_connection *);
void                   delete_ssl_server_socket(ssl_server_connection *);
boolean_t              check_ssl_md5sum(ssl_connection *, char *);
int                    send_ssl_socket(ssl_connection *, void *, size_t, int);
int                    recv_ssl_socket(ssl_connection *, void *, int, int);
ssl_connection        *new_ssl_connection(char *, int);
ssl_connection        *insert_accepted_ssl_socket(ssl_server_connection *);
ssl_server_connection *init_ssl_server(char *, char *);
#ifdef OPENSSL_FIPS
void                   enable_fips_mode();
#endif


#else


typedef void ssl_connection;
typedef void ssl_server_connection;


/* dummy ssl functions */
#define have_ssl()                      0
#define stop_ssl()
#define embed_ssl_socket(x, y)          0
#define embed_accepted_ssl_socket(x, y) 0
#define close_ssl_socket(x)             0
#define close_accepted_ssl_socket(x, y)
#define delete_ssl_socket(x)
#define delete_ssl_server_socket(x)
#define check_ssl_md5sum(x, y)          0
#define send_ssl_socket(a, b, c, d)     0
#define recv_ssl_socket(a, b, c, d)     0
#define new_ssl_connection(x, y)        NULL
#define insert_accepted_ssl_socket(x)   NULL
#define init_ssl_server(x, y)           NULL


#endif


#endif
