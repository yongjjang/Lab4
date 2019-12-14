//
// Created by tt on 19. 12. 15..
//

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>


int id = 0;
char name[20];
char phone_num[20];
char address[20];
char buffer[100];

void input_data(){
    printf("ID : ");
    scanf("%d", &id);
    printf("NAME : ");
    scanf("%s", name);
    printf("PHONE-NUMBER : ");
    scanf("%s", phone_num);
    printf("ADDRESS : ");
    scanf("%s", address);
}

//void read(){
//    if (mysql_query(&mysql, "select * from people")) {
//        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
//        exit(1);
//    }
//    printf("조회가 완료되었습니다.\n");
//}
//
//void create(){
//    input_data();
//    sprintf(buffer, "insert into people values(%d, \"%s\",\"%s\",\"%s\");", id, name, phone_num, address);
//
//    if (mysql_query(&mysql, buffer)) {
//        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
//        exit(1);
//    }
//    printf("생성가 완료되었습니다.\n");
//}
//
//void delete(){
//    printf("INPUT ID\n");
//    printf("ID : ");
//    scanf("%d", &id);
//    sprintf(buffer, "delete from person where id = %d", id);
//    if (mysql_query(&mysql, buffer)) {
//        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
//        exit(1);
//    }
//    printf("삭제가 완료되었습니다.\n");
//}

void main() {
    MYSQL mysql;

    mysql_init(&mysql);
    memset(buffer, 0, sizeof(char));


    if (!mysql_real_connect(&mysql, "localhost", "root", "q", NULL, 0, (char *) NULL, 0)) {
        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
        exit(1);
    }

    if (mysql_query(&mysql, "use people;")) {
        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
        exit(1);
    }

    if (mysql_query(&mysql, "select * from people;")) {
        printf("Error %d: %s\n", mysql_errno(&mysql), mysql_error(&mysql));
        exit(1);
    }else
        printf("조회가 완료되었습니다.\n");

    printf("query info: %s\n", mysql_info(&mysql));
    printf("affected rows: %lu\n", (unsigned long) mysql_affected_rows(&mysql));

    mysql_close(&mysql);
}