#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATIONS 40000
#define MAX_CARS 512

// BST node
typedef struct stationNode {
    int station;
    int stationIndex;
    int backWardLink;
    int jumpsToStart;
    struct stationNode* left;
    struct stationNode* right;
    struct carHeap* maxHeap;
} stationNode;

// MaxHeap used to store the cars
typedef struct carHeap {
    int* heap;
    int size;
}carHeap;

// Dequeue node
typedef struct dequeueNode {
    stationNode* data;
    struct dequeueNode* next;
} dequeueNode;

// Dequeue
typedef struct Dequeue {
    dequeueNode* front;
    dequeueNode * rear;
} Dequeue;

/**
 * station functions
 */

stationNode* nodeCreate(int station) {
    stationNode* node = malloc(sizeof(stationNode));
    node->station = station;
    node->stationIndex = 0;
    node->backWardLink = 0;
    node->jumpsToStart = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

stationNode* nodeInsert(stationNode* root, int station) {
    if (root == NULL) {
        return nodeCreate(station);
    }
    if (station < root->station) {
        root->left = nodeInsert(root->left, station);
    }
    else if (station > root->station) {
        root->right = nodeInsert(root->right, station);
    }
    return root;
}

//funzione che distrugge l'albero
void destroyTree(stationNode* root) {
    if (root == NULL)
        return;
    destroyTree(root->left);
    destroyTree(root->right);
    free(root->maxHeap->heap);
    free(root->maxHeap);
    free(root);
}

stationNode* searchStation (stationNode* root, int station) {
    if (root == NULL)
        return NULL;
    if (root->station == station) {
        return root;
    }
    if (station < root->station) {
        return searchStation(root->left, station);
    }
    return searchStation(root->right, station);
}

// fills an array with the pointers to the stations in the BST (in order) and re-initializes all the values necessary to calculate the shortest path
void inOrderFill(stationNode* root, stationNode** array, int* i) {
    if (root == NULL)
        return;
    inOrderFill(root->left, array, i);
    array[*i] = root;
    root->stationIndex = *i;
    root->jumpsToStart = 0;
    root->backWardLink = 0;
    (*i)++;
    inOrderFill(root->right, array, i);
}

stationNode* findMinimum(stationNode* root) {
    if (root == NULL)
        return NULL;
    else if (root->left != NULL)
        return findMinimum(root->left);
    return root;
}

stationNode* deleteStation(stationNode* root, int station) {
    if (root == NULL)
        return root;

    if (station < root->station)
        root->left = deleteStation(root->left, station);

    else if (station > root->station)
        root->right = deleteStation(root->right, station);

    else {
        // the node has 0 or 1 child
        if (root->left == NULL) {
            stationNode* temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL) {
            stationNode* temp = root->left;
            free(root);
            return temp;
        }
        // the node has 2 children
        stationNode* temp = findMinimum(root->right);
        root->station = temp->station;
        root->maxHeap = temp->maxHeap;
        root->right = deleteStation(root->right, temp->station);
    }
    return root;
}

/**
 * maxHeap functions
 */

carHeap* createCarHeap() {
    carHeap* newHeap = malloc(sizeof(carHeap));
    newHeap->heap = malloc(MAX_CARS * sizeof(int));
    memset(newHeap->heap, 0, MAX_CARS * sizeof(int));
    newHeap->size = 0;
    return newHeap;
}

void swap (int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify(carHeap* maxHeap, int index){
    int max = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < maxHeap->size && maxHeap->heap[left] > maxHeap->heap[max]) {
        max = left;
    }
    if (right < maxHeap->size && maxHeap->heap[right] > maxHeap->heap[max]) {
        max = right;
    }
    if (max != index) {
        swap(&maxHeap->heap[max], &maxHeap->heap[index]);
        maxHeapify(maxHeap, max);
    }
}

void insertCar(carHeap* maxHeap, int car) {
    maxHeap->size++;
    int i = maxHeap->size - 1;
    maxHeap->heap[i] = car;

    while (i != 0 && maxHeap->heap[(i - 1) / 2] < maxHeap->heap[i]) {
        swap(&maxHeap->heap[i], &maxHeap->heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int getMaxCar(carHeap* maxHeap) {
    if(maxHeap->size == 0) {
        return -1;
    }
    return maxHeap->heap[0];
}

int deleteCar(carHeap* maxHeap, int car) {
    int i;
    int found = 0;
    for (i = 0; i < maxHeap->size; i++) {
        if (maxHeap->heap[i] == car) {
            found = 1;
            break;
        }
    }
    if (found == 1) {
        swap(&maxHeap->heap[i], &maxHeap->heap[maxHeap->size - 1]);
        maxHeap->size--;
        maxHeap->heap[maxHeap->size] = 0;
        maxHeapify(maxHeap, i);
        return found;
    }
    return found;
}

/**
 * dequeue functions
 */

Dequeue* createDequeue() {
    Dequeue* dequeue = malloc(sizeof(Dequeue));
    dequeue->front = NULL;
    dequeue->rear = NULL;
    return dequeue;
}

int isDequeueEmpty(Dequeue* dequeue) {
    return (dequeue->front == NULL);
}

void enqueueRear(Dequeue* dequeue, stationNode* data) {
    dequeueNode* node = malloc(sizeof(dequeueNode));
    node->data = data;
    node->next = NULL;

    if (isDequeueEmpty(dequeue)) {
        dequeue->front = node;
        dequeue->rear = node;
    }
    else {
        dequeue->rear->next = node;
        dequeue->rear = node;
    }
}

stationNode* dequeueFront(Dequeue* dequeue) {
    if (isDequeueEmpty(dequeue)) {
        return NULL;
    } else {
        dequeueNode* frontNode = dequeue->front;
        stationNode* data = frontNode->data;

        if (dequeue->front == dequeue->rear) {
            dequeue->front = NULL;
            dequeue->rear = NULL;
        }
        else {
            dequeue->front = dequeue->front->next;
        }
        free(frontNode);
        return data;
    }
}

void destroyDequeue(Dequeue* dequeue) {
    while (!isDequeueEmpty(dequeue))
        dequeueFront(dequeue);

    free(dequeue);
}

void printPath(int* path, int count) {
    int i;
    if (count < 2) {
       fprintf(stdout,"nessun percorso");
       fprintf(stdout,"\n");
       return;
    }
    for (i = count - 1; i > 0; i--) {
        fprintf(stdout,"%d ", path[i]);
    }
    fprintf(stdout,"%d", path[0]);
    fprintf(stdout,"\n");
}

void findShortestPathForward(stationNode* stationRoot, int start, int destination, int stationCount) {
    int index;
    int* stationindex = NULL;

    if (stationRoot == NULL) {
        fprintf(stdout,"nessun percorso");
        fprintf(stdout,"\n");
        return;
    }

    stationNode** stationNodes = (stationNode** ) calloc(stationCount, sizeof(stationNode*));
    inOrderFill(stationRoot, stationNodes, (int*) &stationindex);

    Dequeue* dequeue = createDequeue();

    // array to keep track of visited nodes
    int* visited = (int*) calloc(stationCount, sizeof(int));

    // array of parents
    int* parent = (int*)malloc(stationCount * sizeof(int));
    memset(parent, -1, stationCount * sizeof(int));

    int* path = (int*)malloc(stationCount * sizeof(int));

    stationNode* startNode = searchStation(stationRoot, start);
    stationNode* destinationNode = searchStation(stationRoot, destination);
    enqueueRear(dequeue, startNode);

    visited[startNode->stationIndex] = 1;

    while (!isDequeueEmpty(dequeue)) {
        stationNode *currentNode = dequeueFront(dequeue);

        if (currentNode->station == destination){
            break;
        }
        // reachable stations from the currentNode
        if (currentNode->maxHeap->size > 0) {
            int maxCar = getMaxCar(currentNode->maxHeap);

            for (int i = currentNode->stationIndex + 1; i <= destinationNode->stationIndex && i > 0 ; i++) {
                if (visited[i])
                    continue;

                if (stationNodes[i]->station > currentNode->station + maxCar)
                    break;

                stationNode *nextNode = stationNodes[i];
                index = nextNode->stationIndex;

                enqueueRear(dequeue, nextNode);
                visited[index] = 1;
                parent[index] = currentNode->station;
            }
        }
    }

    int currentNode = destination;
    int count = 0;
    // construct the path from the destination to the start
    while (currentNode != -1 && count <= destinationNode->stationIndex) {
        path[count++] = currentNode;
        int nodeIndex = searchStation(stationRoot, currentNode)->stationIndex;

        if (nodeIndex >= 0 && nodeIndex <= destinationNode->stationIndex)
            currentNode = parent[nodeIndex];
    }

    if (count == 0 || (count > 0 && path[count - 1] != start && path[0] != destination)) {
        fprintf(stdout,"nessun percorso");
        fprintf(stdout, "\n");
    }
    else
       printPath(path, count);

    destroyDequeue(dequeue);
    free(visited);
    free(parent);
    free(path);
    free(stationNodes);
}

void findShortestPathBackwards(stationNode* stationRoot, int start, int destination, int stationCount) {
    int* stationindex = NULL;

    if (stationRoot == NULL) {
        fprintf(stdout,"nessun percorso\n");
        return;
    }

    stationNode** stationNodes = (stationNode**) calloc(stationCount, sizeof(stationNode*));
    inOrderFill(stationRoot, stationNodes, (int *) &stationindex);

    Dequeue* assignBackWardLink = createDequeue();

    int* path = (int*) malloc(stationCount * sizeof(int));
    memset(path, -1, stationCount * sizeof(int));

    // start and dest nodes
    stationNode *startNode = searchStation(stationRoot, start);
    stationNode *destinationNode = searchStation(stationRoot, destination);

    //queue used to assign the backward link to each station
    enqueueRear(assignBackWardLink, startNode);

    // assign the backward link to each station, the jumpsToStart variable is used to decide if the backward link needs to be replaced
    while (!isDequeueEmpty(assignBackWardLink)) {

        stationNode *currentNode = dequeueFront(assignBackWardLink);

        for (int i = currentNode->stationIndex - 1 ;i > 0 && i >= destinationNode->stationIndex; i--) {
            stationNode *nextNode = stationNodes[i];
            if (currentNode->maxHeap->size == 0)
                break;
            int maxCar = getMaxCar(currentNode->maxHeap);
            if (currentNode->station - maxCar > nextNode->station)
                break;
            if (currentNode->station - maxCar <= nextNode->station) {
                if (nextNode->jumpsToStart == 0) {
                    nextNode->backWardLink = currentNode->station;
                    nextNode->jumpsToStart = currentNode->jumpsToStart + 1;
                    enqueueRear(assignBackWardLink, nextNode);
                }
                else{
                    if(currentNode->station < nextNode->backWardLink && currentNode->jumpsToStart + 1 <= nextNode->jumpsToStart) {
                        nextNode->backWardLink = currentNode->station;
                        nextNode->jumpsToStart = currentNode->jumpsToStart + 1;
                    }
                }
            }
        }
    }

    // construct the path from the destinationNode to the start station
    int currentNode = destination;
    int count = 0;

    while (currentNode != 0) {
        path[count++] = currentNode;
        stationNode* nextNode = searchStation(stationRoot,currentNode);
        currentNode = nextNode->backWardLink;
    }
    if (count == 0 || (count > 0 && path[count - 1] != start && path[0] != destination)) {
        fprintf(stdout,"nessun percorso");
        fprintf(stdout,"\n");
    }
    else
        printPath(path, count);

    destroyDequeue(assignBackWardLink);
    free(path);
    free(stationNodes);
}


int main() {
    char input[10000];
    FILE *fp;
    stationNode* stationRoot = NULL;
    stationNode* cacheStation = NULL;
    stationNode* currentStation = NULL;
    int stationCount = 0;

    //fp = fopen("open_extra_gen.txt", "r");
    fp = stdin;

    stationRoot = nodeInsert(stationRoot, -1);
    stationRoot->maxHeap = createCarHeap();
    stationCount++;


    while (fgets(input, sizeof(input), fp) != NULL) {
        char *token = strtok(input, " \n");

        if (strcmp(token, "aggiungi-stazione") == 0) {
            int st = atoi(strtok(NULL, " \n"));
            currentStation = searchStation(stationRoot, st);
            if (currentStation != NULL) {
                fprintf(stdout,"non aggiunta");
                fprintf (stdout,"\n");
            }
            else {
                nodeInsert(stationRoot, st);
                cacheStation = searchStation(stationRoot, st);
                cacheStation->maxHeap = createCarHeap();
                fprintf(stdout,"aggiunta\n");
                stationCount++;
                int numcars = atoi(strtok(NULL, " \n" ));

                for (int i = 0; i < numcars; i++) {
                    int car = atoi(strtok(NULL, " \n"));
                    insertCar(cacheStation->maxHeap, car);
                }
            }
        }
        else if (strcmp( token, "demolisci-stazione") == 0) {
            int st = atoi(strtok(NULL, " \n"));
            currentStation = searchStation(stationRoot, st);

            if (currentStation == NULL) {
                fprintf(stdout,"non demolita");
                fprintf (stdout," \n");
            }
            else {
                free(currentStation->maxHeap->heap);
                free(currentStation->maxHeap);
                stationRoot = deleteStation(stationRoot, st);
                cacheStation = searchStation(stationRoot, st);
                fprintf(stdout,"demolita");
                fprintf (stdout,"\n");
                stationCount--;
            }
        }
        else if (strcmp(token, "pianifica-percorso") == 0) {
            int start = atoi(strtok(NULL, " \n"));
            int destination = atoi(strtok(NULL, " \n"));
            if (start < destination) {
                findShortestPathForward(stationRoot, start, destination,stationCount);
            }
            else if (start > destination){
                findShortestPathBackwards(stationRoot, start, destination,stationCount);
            }
            else if (start == destination){
                fprintf(stdout,"%d ",start);
                fprintf(stdout,"\n");
            }
        }
        else if (strcmp(token, "aggiungi-auto") == 0) {
            int st = atoi(strtok(NULL, " \n"));
            int car = atoi(strtok(NULL, " \n"));
            //cached car insert
            if (cacheStation != NULL && st == cacheStation->station && cacheStation->maxHeap->size < MAX_CARS) {
                insertCar(cacheStation->maxHeap, car);
                fprintf(stdout,"aggiunta");
                fprintf(stdout,"\n");
            }
                //normal car insert
            else {
                currentStation = searchStation(stationRoot, st);
                if (currentStation != NULL && currentStation->maxHeap->size < MAX_CARS) {
                    insertCar(currentStation->maxHeap, car);
                    fprintf(stdout,"aggiunta");
                    fprintf(stdout,"\n");
                } else {
                    fprintf(stdout,"non aggiunta");
                    fprintf(stdout,"\n");
                }
            }
        }
        else if (strcmp(token, "rottama-auto") == 0) {
            int st = atoi(strtok(NULL, " \n"));
            int car = atoi(strtok(NULL, " \n"));
            //cached car delete
            if (cacheStation != NULL && st == cacheStation->station) {
                if (deleteCar(cacheStation->maxHeap, car)) {
                    fprintf(stdout,"rottamata");
                    fprintf (stdout,"\n");
                }
                else {
                    fprintf(stdout,"non rottamata");
                    fprintf (stdout,"\n");
                }
            }
            else if (cacheStation == NULL || st != cacheStation->station) {
                //normal car delete
                currentStation = searchStation(stationRoot, st);
                if (currentStation == NULL) {
                    fprintf(stdout,"non rottamata");
                    fprintf(stdout,"\n");
                } else {
                    if (deleteCar(currentStation->maxHeap, car)) {
                        fprintf(stdout,"rottamata");
                        fprintf(stdout,"\n");
                    } else {
                        fprintf(stdout,"non rottamata");
                        fprintf(stdout,"\n");
                    }
                }
            }
        }
    }
    destroyTree(stationRoot);
    fclose(fp);
    exit (0);
}