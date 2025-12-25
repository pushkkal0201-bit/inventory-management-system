/**
 * INVENTORY MANAGEMENT SYSTEM
 * Author: Pushkkal Gupta
 * Description: Professional CRUD(Create,Read,Update,Delete) Inventory system
 * Features:
 *  - Add / View / Update / Delete items
 *  - Stock In & Stock Out
 *  - Reorder level alerts
 *  - Duplicate item-code prevention
 *  - Inventory value report
 *  - Cross-platform support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CODE 12
#define MAX_NAME 50
#define DB_FILE "inventory_management.dat"
#define TEMP_FILE "temp.dat"

/* -------------------- STRUCT -------------------- */
typedef struct {
    char code[MAX_CODE];
    char name[MAX_NAME];
    float rate;
    float quantity;
    int min;
} 
Stock;

/* -------------------- UTILITIES -------------------- */
void clear_buffer() {
    int c;
    while ((c=getchar())!='\n'&&c!=EOF);
}
//Cross platform feature
void clear_screen() {
#ifdef _WIN32
    system("cls");// for windows
#else
    system("clear");// for Linus/MacOs
#endif
}

/* -------------------- CHECK DUPLICATE -------------------- */
int item_exists(const char *code) {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) return 0;

    Stock s;
    while (fread(&s,sizeof(Stock),1,fp)) 
	{
        if (strcmp(s.code,code)==0) 
		{
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* -------------------- ADD ITEM -------------------- */
void add_item() {
    FILE *fp=fopen(DB_FILE,"ab");
    if (!fp) {
        printf("File error!\n");
        getchar();
        return;
    }

    Stock s;
    clear_screen();
    printf("---ADD NEW ITEM---\n");

    printf("Item Code: ");
    scanf("%11s", s.code);
    clear_buffer();

    if (item_exists(s.code)) {
        printf("Item code already exists!\n");
        fclose(fp);
        getchar();
        return;
    }

    printf("Item Name: ");
    fgets(s.name, MAX_NAME, stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("Price:");
    scanf("%f", &s.rate);
    printf("Quantity:");
    scanf("%f", &s.quantity);
    printf("Reorder Level:");
    scanf("%d", &s.min);
    clear_buffer();

    if (s.rate < 0 || s.quantity < 0 || s.min < 0) {
        printf("Invalid input values!\n");
        fclose(fp);
        getchar();
        return;
    }

    fwrite(&s, sizeof(Stock), 1, fp);
    fclose(fp);
    printf("Item added successfully!\n");
    getchar();
}

/* -------------------- VIEW STOCK -------------------- */
void view_stock() {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) {
        printf("No inventory data found.\n");
        getchar();
        return;
    }

    clear_screen();
    Stock s;
    float total_value = 0;

    printf("%-12s | %-20s | %-8s | %-8s | %-6s\n",
           "CODE", "NAME", "PRICE", "QTY", "STATUS");
    printf("-------------------------------------------------------------\n");

    while (fread(&s, sizeof(Stock), 1, fp)) {
        const char *status = (s.quantity <= s.min) ? "LOW" : "";
        printf("%-12s | %-20s | %-8.2f | %-8.2f | %-6s\n",
               s.code, s.name, s.rate, s.quantity, status);
        total_value = total_value+ s.rate * s.quantity;
    }

    fclose(fp);
    printf("\nTotal Inventory Value: %.2f\n", total_value);
    printf("Press Enter to return...");
    getchar();
}

/* -------------------- SEARCH ITEM -------------------- */
void search_item() {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) return;

    char code[MAX_CODE];
    Stock s;
    int found =0;

    printf("Enter Item Code:");
    scanf("%11s", code);
    clear_buffer();

    while (fread(&s, sizeof(Stock), 1, fp)) {
        if (strcmp(s.code, code) == 0) {
            printf("\nCode: %s\nName: %s\nPrice: %.2f\nQty: %.2f\nReorder: %d\n",
                   s.code, s.name, s.rate, s.quantity, s.min);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Item not found.\n");

    fclose(fp);
    getchar();
}

/* -------------------- UPDATE ITEM -------------------- */
void update_item() {
    FILE *fp = fopen(DB_FILE, "rb");
    FILE *ft = fopen(TEMP_FILE, "wb");
    if (!fp || !ft) return;

    char code[MAX_CODE];
    Stock s;
    int found = 0;

    printf("Enter Item Code to Update:");
    scanf("%11s", code);
    clear_buffer();

    while (fread(&s, sizeof(Stock), 1, fp)) {
        if (strcmp(s.code, code) == 0) {
            found = 1;
            printf("New Price:");
            scanf("%f", &s.rate);
            printf("New Reorder Level:");
            scanf("%d", &s.min);
            clear_buffer();
        }
        fwrite(&s,sizeof(Stock),1,ft);
    }

    fclose(fp);
    fclose(ft);

    if (found) {
        remove(DB_FILE);
        rename(TEMP_FILE, DB_FILE);
        printf("Item updated.\n");
    } else {
        remove(TEMP_FILE);
        printf("Item not found.\n");
    }
    getchar();
}

/* -------------------- STOCK IN/OUT -------------------- */
void stock_transaction(int is_stock_in) {
    FILE *fp = fopen(DB_FILE, "rb");
    FILE *ft = fopen(TEMP_FILE, "wb");
    if (!fp || !ft) return;

    char code[MAX_CODE];
    float qty;
    Stock s;
    int found = 0;

    printf("Item Code:");
    scanf("%11s", code);
    printf("Quantity:");
    scanf("%f", &qty);
    clear_buffer();

    while (fread(&s, sizeof(Stock), 1, fp)) {
        if (strcmp(s.code, code) == 0) {
            found = 1;
            if (!is_stock_in && qty > s.quantity) {
                printf("Insufficient stock!\n");
            } else {
                s.quantity += is_stock_in ? qty : -qty;
            }
        }
        fwrite(&s, sizeof(Stock), 1, ft);
    }

    fclose(fp);
    fclose(ft);

    if (found) {
        remove(DB_FILE);
        rename(TEMP_FILE, DB_FILE);
        printf("Stock updated.\n");
    } else {
        remove(TEMP_FILE);
        printf("Item not found.\n");
    }
    getchar();
}

/* -------------------- DELETE ITEM -------------------- */
void delete_item() {
    FILE *fp = fopen(DB_FILE, "rb");
    FILE *ft = fopen(TEMP_FILE, "wb");
    if (!fp || !ft) return;

    char code[MAX_CODE];
    Stock s;
    int found = 0;

    printf("Enter Item Code to Delete:");
    scanf("%11s",code);
    clear_buffer();

    while (fread(&s,sizeof(Stock),1,fp)) {
        if (strcmp(s.code, code) == 0)
            found=1;
        else
            fwrite(&s, sizeof(Stock),1,ft);
    }

    fclose(fp);
    fclose(ft);

    if (found) 
	{
        remove(DB_FILE);
        rename(TEMP_FILE, DB_FILE);
        printf("Item deleted.\n");
    } else {
        remove(TEMP_FILE);
        printf("Item not found.\n");
    }
    getchar();
}

/* -------------------- MAIN MENU -------------------- */
int main() {
    int choice;
    while (1) {
        clear_screen();
        printf("========= INVENTORY MANAGEMENT SYSTEM =========\n");
        printf("1. Add Item\n");
        printf("2. View Stock\n");
        printf("3. Search Item\n");
        printf("4. Update Item\n");
        printf("5. Stock In\n");
        printf("6. Stock Out\n");
        printf("7. Delete Item\n");
        printf("0. Exit\n");
        printf("Select: ");

        scanf("%d", &choice);
        clear_buffer();

        switch (choice) {
            case 1: add_item(); break;
            case 2: view_stock(); break;
            case 3: search_item(); break;
            case 4: update_item(); break;
            case 5: stock_transaction(1); break;
            case 6: stock_transaction(0); break;
            case 7: delete_item(); break;
            case 0: exit(0);
            default: printf("Invalid choice!\n"); 
			getchar();
        }
    }
}

