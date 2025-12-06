#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define USERFILE "users.txt"
#define STUDENT_FILE "students.txt"
#define TICKET_FILE "tickets.txt"
#define MAX_LINE 512
#define MAX_FIELD 128

/* Utilities */
void strip_newline(char *s) {
    size_t n = strlen(s);
    if (n == 0) return;
    if (s[n-1] == '\n') s[n-1] = '\0';
    if (n > 1 && s[n-2] == '\r') s[n-2] = '\0';
}

/* -------------------- Login system -------------------- */
int user_exists(const char *username) {
    FILE *f = fopen(USERFILE, "r");
    if (!f) return 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);
        char tmp[MAX_LINE]; strcpy(tmp, line);
        char *u = strtok(tmp, ",");
        if (!u) continue;
        if (strcmp(u, username) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int register_user() {
    char username[MAX_FIELD];
    char password[MAX_FIELD];
    char role[MAX_FIELD];

    printf("Register new user\n");
    printf("Username: ");
    if (!fgets(username, sizeof(username), stdin)) return 0;
    strip_newline(username);
    if (strlen(username) == 0) {
        printf("Username cannot be empty.\n");
        return 0;
    }
    if (strchr(username, ',') != NULL) {
        printf("Username cannot contain ',' character.\n");
        return 0;
    }

    if (user_exists(username)) {
        printf("User '%s' already exists. Try login.\n", username);
        return 0;
    }

    printf("Password: ");
    if (!fgets(password, sizeof(password), stdin)) return 0;
    strip_newline(password);
    if (strchr(password, ',') != NULL) {
        printf("Password cannot contain ',' character.\n");
        return 0;
    }

    printf("Role (student/admin): ");
    if (!fgets(role, sizeof(role), stdin)) return 0;
    strip_newline(role);
    for (char *p=role; *p; ++p) *p = tolower((unsigned char)*p);
    if (strcmp(role, "student") != 0 && strcmp(role, "admin") != 0) {
        printf("Invalid role. Defaulting to 'student'.\n");
        strcpy(role, "student");
    }

    FILE *f = fopen(USERFILE, "a");
    if (!f) {
        perror("Unable to open user file for writing");
        return 0;
    }
    fprintf(f, "%s,%s,%s\n", username, password, role);
    fclose(f);
    printf("Registration successful for '%s' (%s).\n", username, role);
    return 1;
}

/* Forward declaration for ticket menu */
int ticket_menu(void);

int login_user() {
    char username[MAX_FIELD];
    char password[MAX_FIELD];

    printf("Login\n");
    printf("Username: ");
    if (!fgets(username, sizeof(username), stdin)) return 0;
    strip_newline(username);

    printf("Password: ");
    if (!fgets(password, sizeof(password), stdin)) return 0;
    strip_newline(password);

    FILE *f = fopen(USERFILE, "r");
    if (!f) {
        printf("No users registered yet. Please register first.\n");
        return 0;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);
        char saved[MAX_LINE];
        strcpy(saved, line);
        char *u = strtok(saved, ",");
        char *p = strtok(NULL, ",");
        char *r = strtok(NULL, ",");
        if (!u || !p) continue;
        if (strcmp(u, username) == 0 && strcmp(p, password) == 0) {
            printf("Login successful. Welcome %s (%s)!\n", u, r ? r : "student");
            fclose(f);
            /* After successful login, open ticketing menu */
            ticket_menu();
            return 1;
        }
    }
    fclose(f);
    printf("Invalid username or password.\n");
    return 0;
}

/* -------------------- Ticket system -------------------- */
int student_exists(const char *rollid) {
    FILE *f = fopen(STUDENT_FILE, "r");
    if (!f) return 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);
        char tmp[MAX_LINE];
        strcpy(tmp, line);
        char *r = strtok(tmp, ",");
        if (!r) continue;
        if (strcmp(r, rollid) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int register_student() {
    char rollid[MAX_FIELD], name[MAX_FIELD], classroom[MAX_FIELD], subject[MAX_FIELD];
    printf("Register student\n");
    printf("Roll ID: "); if (!fgets(rollid, sizeof(rollid), stdin)) return 0; strip_newline(rollid);
    if (strlen(rollid) == 0) { printf("Roll ID cannot be empty.\n"); return 0; }
    if (student_exists(rollid)) { printf("Roll ID '%s' already registered.\n", rollid); return 0; }

    printf("Name: "); if (!fgets(name, sizeof(name), stdin)) return 0; strip_newline(name);
    printf("Classroom (e.g. 10A): "); if (!fgets(classroom, sizeof(classroom), stdin)) return 0; strip_newline(classroom);
    printf("Subject: "); if (!fgets(subject, sizeof(subject), stdin)) return 0; strip_newline(subject);

    FILE *f = fopen(STUDENT_FILE, "a");
    if (!f) { perror("Unable to open student file"); return 0; }
    fprintf(f, "%s,%s,%s,%s\n", rollid, name, classroom, subject);
    fclose(f);
    printf("Student '%s' registered.\n", name);
    return 1;
}

int raise_complaint() {
    char rollid[MAX_FIELD], complaint[MAX_LINE];
    printf("Raise complaint\n");
    printf("Roll ID: "); if (!fgets(rollid, sizeof(rollid), stdin)) return 0; strip_newline(rollid);
    if (!student_exists(rollid)) { printf("Roll ID not found. Please register first.\n"); return 0; }

    /* fetch student details */
    char name[MAX_FIELD] = "";
    char classroom[MAX_FIELD] = "";
    char subject[MAX_FIELD] = "";
    FILE *fs = fopen(STUDENT_FILE, "r");
    if (fs) {
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), fs)) {
            strip_newline(line);
            char tmp[MAX_LINE]; strcpy(tmp, line);
            char *r = strtok(tmp, ",");
            char *n = strtok(NULL, ",");
            char *c = strtok(NULL, ",");
            char *s = strtok(NULL, ",");
            if (r && strcmp(r, rollid) == 0) {
                if (n) strncpy(name, n, sizeof(name)-1);
                if (c) strncpy(classroom, c, sizeof(classroom)-1);
                if (s) strncpy(subject, s, sizeof(subject)-1);
                break;
            }
        }
        fclose(fs);
    }

    printf("Complaint (one line): "); if (!fgets(complaint, sizeof(complaint), stdin)) return 0; strip_newline(complaint);

    /* generate ticket id as next number */
    int next_id = 1;
    FILE *ft = fopen(TICKET_FILE, "r");
    if (ft) {
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), ft)) {
            next_id++;
        }
        fclose(ft);
    }

    ft = fopen(TICKET_FILE, "a");
    if (!ft) { perror("Unable to open ticket file"); return 0; }
    fprintf(ft, "%d,%s,%s,%s,%s\n", next_id, rollid, name, classroom, complaint);
    fclose(ft);
    printf("Complaint submitted with ticket ID %d.\n", next_id);
    return 1;
}

int search_classroom() {
    printf("Search classroom\n");
    printf("1) Search by Roll ID\n2) Search by Classroom name\nChoose: ");
    char choice[8]; if (!fgets(choice, sizeof(choice), stdin)) return 0; strip_newline(choice);
    if (choice[0] == '1') {
        char rollid[MAX_FIELD]; printf("Roll ID: "); if (!fgets(rollid, sizeof(rollid), stdin)) return 0; strip_newline(rollid);
        FILE *f = fopen(STUDENT_FILE, "r");
        if (!f) { printf("No students registered yet.\n"); return 0; }
        char line[MAX_LINE];
        int found = 0;
        while (fgets(line, sizeof(line), f)) {
            strip_newline(line);
            char tmp[MAX_LINE]; strcpy(tmp, line);
            char *r = strtok(tmp, ",");
            char *n = strtok(NULL, ",");
            char *c = strtok(NULL, ",");
            char *s = strtok(NULL, ",");
            if (r && strcmp(r, rollid) == 0) {
                printf("Roll: %s\nName: %s\nClassroom: %s\nSubject: %s\n", r, n? n: "", c? c: "", s? s: "");
                found = 1; break;
            }
        }
        fclose(f);
        if (!found) printf("Student not found.\n");
    } else if (choice[0] == '2') {
        char classroom[MAX_FIELD]; printf("Classroom name: "); if (!fgets(classroom, sizeof(classroom), stdin)) return 0; strip_newline(classroom);
        FILE *f = fopen(STUDENT_FILE, "r");
        if (!f) { printf("No students registered yet.\n"); return 0; }
        char line[MAX_LINE];
        int any = 0;
        printf("Students in classroom %s:\n", classroom);
        while (fgets(line, sizeof(line), f)) {
            strip_newline(line);
            char tmp[MAX_LINE]; strcpy(tmp, line);
            char *r = strtok(tmp, ",");
            char *n = strtok(NULL, ",");
            char *c = strtok(NULL, ",");
            char *s = strtok(NULL, ",");
            if (c && strcmp(c, classroom) == 0) {
                printf("- %s (Roll: %s, Subject: %s)\n", n? n: "", r? r: "", s? s: "");
                any = 1;
            }
        }
        fclose(f);
        if (!any) printf("No students found in that classroom.\n");
    } else {
        printf("Invalid choice.\n");
    }
    return 1;
}

int update_subject() {
    char rollid[MAX_FIELD]; char new_subject[MAX_FIELD];
    printf("Update subject (authorization by Roll ID)\n");
    printf("Roll ID: "); if (!fgets(rollid, sizeof(rollid), stdin)) return 0; strip_newline(rollid);
    if (!student_exists(rollid)) { printf("Roll ID not found.\n"); return 0; }
    printf("New subject: "); if (!fgets(new_subject, sizeof(new_subject), stdin)) return 0; strip_newline(new_subject);

    FILE *f = fopen(STUDENT_FILE, "r");
    if (!f) { printf("No student file.\n"); return 0; }
    FILE *tmp = fopen("students.tmp", "w");
    if (!tmp) { fclose(f); perror("Unable to open temp file"); return 0; }
    char line[MAX_LINE];
    int updated = 0;
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);
        char copy[MAX_LINE]; strcpy(copy, line);
        char *r = strtok(copy, ",");
        char *n = strtok(NULL, ",");
        char *c = strtok(NULL, ",");
        char *s = strtok(NULL, ",");
        if (r && strcmp(r, rollid) == 0) {
            fprintf(tmp, "%s,%s,%s,%s\n", r, n? n: "", c? c: "", new_subject);
            updated = 1;
        } else {
            fprintf(tmp, "%s\n", line);
        }
    }
    fclose(f); fclose(tmp);
    if (updated) {
        remove(STUDENT_FILE);
        rename("students.tmp", STUDENT_FILE);
        printf("Subject updated.\n");
    } else {
        remove("students.tmp");
        printf("Student not found to update.\n");
    }
    return updated;
}

int delete_from_subject() {
    char rollid[MAX_FIELD]; char subject[MAX_FIELD];
    printf("Delete name from subject (authorization by Roll ID)\n");
    printf("Roll ID: "); if (!fgets(rollid, sizeof(rollid), stdin)) return 0; strip_newline(rollid);
    if (!student_exists(rollid)) { printf("Roll ID not found.\n"); return 0; }
    printf("Subject to delete from: "); if (!fgets(subject, sizeof(subject), stdin)) return 0; strip_newline(subject);

    FILE *f = fopen(STUDENT_FILE, "r");
    if (!f) { printf("No student file.\n"); return 0; }
    FILE *tmp = fopen("students.tmp", "w");
    if (!tmp) { fclose(f); perror("Unable to open temp file"); return 0; }
    char line[MAX_LINE];
    int deleted = 0;
    while (fgets(line, sizeof(line), f)) {
        strip_newline(line);
        char copy[MAX_LINE]; strcpy(copy, line);
        char *r = strtok(copy, ",");
        char *n = strtok(NULL, ",");
        char *c = strtok(NULL, ",");
        char *s = strtok(NULL, ",");
        if (r && strcmp(r, rollid) == 0 && s && strcmp(s, subject) == 0) {
            /* skip writing this line -> delete */
            deleted = 1; continue;
        } else {
            fprintf(tmp, "%s\n", line);
        }
    }
    fclose(f); fclose(tmp);
    if (deleted) {
        remove(STUDENT_FILE);
        rename("students.tmp", STUDENT_FILE);
        printf("Student removed from subject.\n");
    } else {
        remove("students.tmp");
        printf("No matching record found to delete.\n");
    }
    return deleted;
}

/* Combined ticket menu that returns to login on logout */
int ticket_menu(void) {
    while (1) {
        printf("\nTicketing Menu:\n");
        printf("1) Register student\n2) Raise complaint\n3) Search classroom\n4) Update subject\n5) Delete name from subject\n6) Logout\nChoose: ");
        char input[8]; if (!fgets(input, sizeof(input), stdin)) return 0; strip_newline(input);
        if (input[0] == '1') register_student();
        else if (input[0] == '2') raise_complaint();
        else if (input[0] == '3') search_classroom();
        else if (input[0] == '4') update_subject();
        else if (input[0] == '5') delete_from_subject();
        else if (input[0] == '6') { printf("Logging out of ticketing menu.\n"); break; }
        else printf("Invalid option.\n");
    }
    return 1;
}

/* -------------------- Program entry -------------------- */
int main(void) {
    printf("Welcome to the combined Login + Ticketing app\n");
    while (1) {
        printf("\nMenu:\n1) Login\n2) Register\n3) Exit\nChoose: ");
        char input[8];
        if (!fgets(input, sizeof(input), stdin)) break;
        strip_newline(input);
        if (input[0] == '1') {
            login_user();
        } else if (input[0] == '2') {
            register_user();
        } else if (input[0] == '3') {
            printf("Bye!\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
    return 0;
}
