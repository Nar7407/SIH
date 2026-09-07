#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int arr[MAX];
int top = -1;

void pushArray(int val) {
    if (top == MAX - 1) {
        printf("\nStack Overflow! Cannot push %d\n", val);
        return;
    }
    top++;
    arr[top] = val;
    printf("\n%d pushed onto stack\n", val);
}

void popArray() {
    if (top == -1) {
        printf("\nStack Underflow! Stack is empty\n");
        return;
    }
    printf("\n%d popped from stack\n", arr[top]);
    top--;
}

void peekArray() {
    if (top == -1) {
        printf("\nStack is empty\n");
        return;
    }
    printf("\nTop element: %d\n", arr[top]);
}

void displayArray() {
    if (top == -1) {
        printf("\nStack is empty\n");
        return;
    }
    printf("\nStack (top to bottom): ");
    for (int i = top; i >= 0; i--) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *listTop = NULL;

void pushList(int val) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = val;
    newNode->next = listTop;
    listTop = newNode;
    printf("\n%d pushed onto stack\n", val);
}

void popList() {
    if (listTop == NULL) {
        printf("\nStack Underflow! Stack is empty\n");
        return;
    }
    Node *temp = listTop;
    printf("\n%d popped from stack\n", temp->data);
    listTop = temp->next;
    free(temp);
}

void peekList() {
    if (listTop == NULL) {
        printf("\nStack is empty\n");
        return;
    }
    printf("\nTop element: %d\n", listTop->data);
}

void displayList() {
    if (listTop == NULL) {
        printf("\nStack is empty\n");
        return;
    }
    printf("\nStack (top to bottom): ");
    Node *temp = listTop;
    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

void freeList() {
    while (listTop != NULL) {
        Node *temp = listTop;
        listTop = temp->next;
        free(temp);
    }
}

int main() {
    int impl, choice, val;

    printf("----- STACK IMPLEMENTATION -----\n");
    printf("1. Using Array\n");
    printf("2. Using Linked List\n");
    printf("Enter your choice: ");
    scanf("%d", &impl);

    do {
        printf("\n========= MENU =========\n");
        printf("1. Push\n");
        printf("2. Pop\n");
        printf("3. Peek\n");
        printf("4. Display\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to push: ");
                scanf("%d", &val);
                if (impl == 1)
                    pushArray(val);
                else
                    pushList(val);
                break;
            case 2:
                if (impl == 1)
                    popArray();
                else
                    popList();
                break;
            case 3:
                if (impl == 1)
                    peekArray();
                else
                    peekList();
                break;
            case 4:
                if (impl == 1)
                    displayArray();
                else
                    displayList();
                break;
            case 5:
                printf("\nExiting...\n");
                break;
            default:
                printf("\nInvalid choice! Try again.\n");
        }
    } while (choice != 5);

    freeList();
    return 0;
}
