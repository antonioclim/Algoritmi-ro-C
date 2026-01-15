/**
 * =============================================================================
 * SOLUȚIE: TEMA 1 - Sistem de Gestionare a Studenților
 * =============================================================================
 *
 * NOTĂ: Acest fișier este doar pentru instructori!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    int nr_matricol;
    char nume[50];
    char prenume[50];
    char facultate[100];
    int an_studiu;
    float medie;
    struct Student *left;
    struct Student *right;
} Student;

typedef struct {
    Student *root;
    size_t size;
} StudentDB;

/* Funcții de creare */
Student* create_student(int nr_matricol, const char *nume, const char *prenume,
                        const char *facultate, int an_studiu, float medie) {
    Student *s = (Student*)malloc(sizeof(Student));
    if (!s) return NULL;
    
    s->nr_matricol = nr_matricol;
    strncpy(s->nume, nume, 49); s->nume[49] = '\0';
    strncpy(s->prenume, prenume, 49); s->prenume[49] = '\0';
    strncpy(s->facultate, facultate, 99); s->facultate[99] = '\0';
    s->an_studiu = an_studiu;
    s->medie = medie;
    s->left = s->right = NULL;
    
    return s;
}

StudentDB* create_db(void) {
    StudentDB *db = (StudentDB*)malloc(sizeof(StudentDB));
    if (db) {
        db->root = NULL;
        db->size = 0;
    }
    return db;
}

/* Inserare */
Student* insert_student(Student *node, Student *new_student) {
    if (node == NULL) return new_student;
    
    if (new_student->nr_matricol < node->nr_matricol) {
        node->left = insert_student(node->left, new_student);
    } else if (new_student->nr_matricol > node->nr_matricol) {
        node->right = insert_student(node->right, new_student);
    } else {
        /* Actualizare date existente */
        strcpy(node->nume, new_student->nume);
        strcpy(node->prenume, new_student->prenume);
        strcpy(node->facultate, new_student->facultate);
        node->an_studiu = new_student->an_studiu;
        node->medie = new_student->medie;
        free(new_student);
        return node;
    }
    
    return node;
}

void db_add_student(StudentDB *db, int nr_matricol, const char *nume, 
                    const char *prenume, const char *facultate, 
                    int an_studiu, float medie) {
    Student *s = create_student(nr_matricol, nume, prenume, facultate, an_studiu, medie);
    if (s) {
        db->root = insert_student(db->root, s);
        db->size++;
        printf("Student adăugat: [%d] %s %s\n", nr_matricol, nume, prenume);
    }
}

/* Căutare */
Student* search_student(Student *node, int nr_matricol) {
    if (node == NULL || node->nr_matricol == nr_matricol) return node;
    
    if (nr_matricol < node->nr_matricol)
        return search_student(node->left, nr_matricol);
    return search_student(node->right, nr_matricol);
}

/* Ștergere */
Student* find_min_student(Student *node) {
    while (node && node->left) node = node->left;
    return node;
}

Student* delete_student(Student *node, int nr_matricol) {
    if (node == NULL) return NULL;
    
    if (nr_matricol < node->nr_matricol) {
        node->left = delete_student(node->left, nr_matricol);
    } else if (nr_matricol > node->nr_matricol) {
        node->right = delete_student(node->right, nr_matricol);
    } else {
        if (node->left == NULL) {
            Student *temp = node->right;
            free(node);
            return temp;
        }
        if (node->right == NULL) {
            Student *temp = node->left;
            free(node);
            return temp;
        }
        
        Student *succ = find_min_student(node->right);
        node->nr_matricol = succ->nr_matricol;
        strcpy(node->nume, succ->nume);
        strcpy(node->prenume, succ->prenume);
        strcpy(node->facultate, succ->facultate);
        node->an_studiu = succ->an_studiu;
        node->medie = succ->medie;
        node->right = delete_student(node->right, succ->nr_matricol);
    }
    return node;
}

/* Afișare */
void print_student(Student *s) {
    printf("[%d] %s %s - %s, An %d, Media: %.2f\n",
           s->nr_matricol, s->nume, s->prenume, 
           s->facultate, s->an_studiu, s->medie);
}

void print_inorder(Student *node) {
    if (node == NULL) return;
    print_inorder(node->left);
    print_student(node);
    print_inorder(node->right);
}

/* Statistici */
void calc_stats(Student *node, int *count, float *sum, Student **best) {
    if (node == NULL) return;
    
    calc_stats(node->left, count, sum, best);
    
    (*count)++;
    *sum += node->medie;
    if (*best == NULL || node->medie > (*best)->medie) {
        *best = node;
    }
    
    calc_stats(node->right, count, sum, best);
}

void count_by_year(Student *node, int *years) {
    if (node == NULL) return;
    count_by_year(node->left, years);
    if (node->an_studiu >= 1 && node->an_studiu <= 4) {
        years[node->an_studiu - 1]++;
    }
    count_by_year(node->right, years);
}

void print_stats(StudentDB *db) {
    printf("\n=== STATISTICI ===\n");
    printf("Total studenți: %zu\n", db->size);
    
    int count = 0;
    float sum = 0;
    Student *best = NULL;
    calc_stats(db->root, &count, &sum, &best);
    
    if (count > 0) {
        printf("Media generală: %.2f\n", sum / count);
        if (best) {
            printf("Bursier (cea mai mare medie): %s %s (%.2f)\n",
                   best->nume, best->prenume, best->medie);
        }
    }
    
    int years[4] = {0};
    count_by_year(db->root, years);
    printf("\nStudenți pe ani:\n");
    for (int i = 0; i < 4; i++) {
        printf("  Anul %d: %d studenți\n", i + 1, years[i]);
    }
}

/* Filtrare */
void filter_by_grade(Student *node, float min_grade) {
    if (node == NULL) return;
    filter_by_grade(node->left, min_grade);
    if (node->medie >= min_grade) {
        print_student(node);
    }
    filter_by_grade(node->right, min_grade);
}

void filter_by_faculty(Student *node, const char *faculty) {
    if (node == NULL) return;
    filter_by_faculty(node->left, faculty);
    if (strcmp(node->facultate, faculty) == 0) {
        print_student(node);
    }
    filter_by_faculty(node->right, faculty);
}

void filter_by_range(Student *node, int low, int high) {
    if (node == NULL) return;
    
    if (node->nr_matricol > low) {
        filter_by_range(node->left, low, high);
    }
    
    if (node->nr_matricol >= low && node->nr_matricol <= high) {
        print_student(node);
    }
    
    if (node->nr_matricol < high) {
        filter_by_range(node->right, low, high);
    }
}

/* Eliberare */
void free_students(Student *node) {
    if (node == NULL) return;
    free_students(node->left);
    free_students(node->right);
    free(node);
}

void destroy_db(StudentDB *db) {
    if (db) {
        free_students(db->root);
        free(db);
    }
}

/* Main */
int main(void) {
    StudentDB *db = create_db();
    
    printf("=== SISTEM GESTIONARE STUDENȚI (SOLUȚIE) ===\n\n");
    
    /* Date de test */
    db_add_student(db, 12345, "Popescu", "Ion", "CSIE", 2, 8.75);
    db_add_student(db, 12340, "Ionescu", "Maria", "CSIE", 1, 9.50);
    db_add_student(db, 12350, "Georgescu", "Andrei", "ASE", 3, 7.25);
    db_add_student(db, 12342, "Dumitrescu", "Elena", "CSIE", 2, 9.00);
    db_add_student(db, 12360, "Vasilescu", "Dan", "ASE", 4, 8.00);
    
    printf("\n=== TOȚI STUDENȚII ===\n");
    print_inorder(db->root);
    
    print_stats(db);
    
    printf("\n=== STUDENȚI CU MEDIA >= 8.5 ===\n");
    filter_by_grade(db->root, 8.5);
    
    printf("\n=== STUDENȚI DIN CSIE ===\n");
    filter_by_faculty(db->root, "CSIE");
    
    printf("\n=== STUDENȚI CU NR. MATRICOL 12340-12350 ===\n");
    filter_by_range(db->root, 12340, 12350);
    
    printf("\n=== CĂUTARE 12345 ===\n");
    Student *found = search_student(db->root, 12345);
    if (found) print_student(found);
    
    printf("\n=== ȘTERGERE 12350 ===\n");
    db->root = delete_student(db->root, 12350);
    db->size--;
    
    printf("\n=== DUPĂ ȘTERGERE ===\n");
    print_inorder(db->root);
    
    destroy_db(db);
    printf("\nProgram terminat.\n");
    
    return 0;
}
