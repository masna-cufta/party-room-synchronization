#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

pthread_mutex_t mutex_lock;
pthread_cond_t can_enter_students;
pthread_cond_t room_empty;

int students_in_room = 0;
bool partybreaker_inside = false;
int finished_students = 0;
int total_students;  // number of student threads

int random_between(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void* student(void* arg) {
    int id = *(int*)arg;
    free(arg);

    usleep(random_between(100, 500) * 1000);

    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex_lock);

        while (partybreaker_inside) {
            pthread_cond_wait(&can_enter_students, &mutex_lock);
        }

        students_in_room++;
        printf("student %d entered the room (%d. time)\n", id, i + 1);

        pthread_mutex_unlock(&mutex_lock);

        usleep(random_between(1000, 2000) * 1000);

        pthread_mutex_lock(&mutex_lock);

        students_in_room--;
        printf("student %d left the room\n", id);

        if (students_in_room == 0) {
            pthread_cond_signal(&room_empty);
        }

        pthread_mutex_unlock(&mutex_lock);

        usleep(random_between(1000, 2000) * 1000);
    }

    pthread_mutex_lock(&mutex_lock);
    finished_students++;
    pthread_cond_signal(&can_enter_students);
    pthread_cond_signal(&room_empty);
    pthread_mutex_unlock(&mutex_lock);

    return nullptr;
}

void* partybreaker(void*) {
    while (true) {
        usleep(random_between(100, 1000) * 1000);

        pthread_mutex_lock(&mutex_lock);

        while (students_in_room < 3 && finished_students < total_students) {
            pthread_cond_wait(&can_enter_students, &mutex_lock);
        }

        if (finished_students == total_students && students_in_room == 0) {
            pthread_mutex_unlock(&mutex_lock);
            break;
        }

        partybreaker_inside = true;
        printf("partybreaker entered the room\n");

        pthread_mutex_unlock(&mutex_lock);

        pthread_mutex_lock(&mutex_lock);

        while (students_in_room > 0) {
            pthread_cond_wait(&room_empty, &mutex_lock);
        }

        partybreaker_inside = false;
        printf("partybreaker left the room\n");

        pthread_cond_broadcast(&can_enter_students);
        pthread_mutex_unlock(&mutex_lock);
    }

    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect usage: provide number of students\n");
        return 1;
    }

    total_students = atoi(argv[1]);
    if (total_students < 4) {
        printf("Number of students must be at least 4\n");
        return 1;
    }

    srand(time(NULL));

    pthread_mutex_init(&mutex_lock, nullptr);
    pthread_cond_init(&can_enter_students, nullptr);
    pthread_cond_init(&room_empty, nullptr);

    pthread_t* students = new pthread_t[total_students];
    pthread_t breaker;

    for (int i = 0; i < total_students; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i + 1;

        if (pthread_create(&students[i], nullptr, student, id) != 0) {
            perror("Error creating thread");
            exit(1);
        }
    }

    if (pthread_create(&breaker, nullptr, partybreaker, nullptr) != 0) {
        perror("Error creating thread");
        exit(1);
    }

    for (int i = 0; i < total_students; i++) {
        pthread_join(students[i], nullptr);
    }

    pthread_join(breaker, nullptr);

    delete[] students;

    pthread_mutex_destroy(&mutex_lock);
    pthread_cond_destroy(&can_enter_students);
    pthread_cond_destroy(&room_empty);

    return 0;
}
