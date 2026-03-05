/* Program name: final_DawnRx.cpp
 *  Author: Sahar Musleh
 *  Date last updated: 2026-02-28
 *  Purpose: Text-based menu program for DawnRx Community Pharmacy using SQLite.
 */

#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <sqlite3.h>

using std::cin; using std::cout; using std::endl; using std::string;

// Small input helpers
void clearInput(){ cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
string prompt(const string& label){ cout<<label; string s; std::getline(cin,s); return s; }
int promptInt(const string& label){ while(true){ cout<<label; int v; if(cin>>v){ clearInput(); return v; } clearInput(); cout<<"Invalid number. Try again.\n"; } }
double promptDouble(const string& label){ while(true){ cout<<label; double v; if(cin>>v){ clearInput(); return v; } clearInput(); cout<<"Invalid number. Try again.\n"; } }

// Prepare/Finalize wrapper
struct Stmt{
    sqlite3_stmt* stmt=nullptr;
    Stmt(sqlite3* db, const char* sql){ if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr)!=SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db)); }
    ~Stmt(){ if(stmt) sqlite3_finalize(stmt); }
};

// Open DB + enable FKs
sqlite3* openDb(const char* path){
    sqlite3* db=nullptr;
    if(sqlite3_open(path,&db)!=SQLITE_OK) throw std::runtime_error("Cannot open DB");
    char* err=nullptr;
    if(sqlite3_exec(db,"PRAGMA foreign_keys = ON;",nullptr,nullptr,&err)!=SQLITE_OK){
        string m = err?err:"PRAGMA failed"; if(err) sqlite3_free(err); throw std::runtime_error(m);
    }
    return db;
}

// Exists helper
bool existsById(sqlite3* db, const string& table, const string& key, int id){
    string sql="SELECT 1 FROM "+table+" WHERE "+key+"=? LIMIT 1;";
    Stmt s(db, sql.c_str());
    sqlite3_bind_int(s.stmt,1,id);
    return sqlite3_step(s.stmt)==SQLITE_ROW;
}

// ADD
void addCustomer(sqlite3* db){
    cout<<"\n== Add Customer ==\n";
    int id=promptInt("CUSTOMER_ID: "); string first=prompt("FIRST_NAME: "); string last=prompt("LAST_NAME: ");
    string dob=prompt("DOB (YYYY-MM-DD): "); string phone=prompt("PHONE: "); string email=prompt("EMAIL: "); string addr=prompt("ADDRESS: ");
    const char* sql="INSERT INTO Customers (CUSTOMER_ID,FIRST_NAME,LAST_NAME,DOB,PHONE,EMAIL,ADDRESS) VALUES (?,?,?,?,?,?,?);";
    try{ Stmt s(db,sql);
        sqlite3_bind_int(s.stmt,1,id);
        sqlite3_bind_text(s.stmt,2,first.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,3,last.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,4,dob.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,5,phone.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,6,email.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,7,addr.c_str(),-1,SQLITE_TRANSIENT);
        if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        cout<<"Customer added.\n";
    }catch(const std::exception& e){ cout<<"Error: "<<e.what()<<"\n"; }
}

void addPrescription(sqlite3* db){
    cout<<"\n== Add Prescription ==\n";
    int id=promptInt("PRESCRIPTION_ID: "); int cust=promptInt("CUSTOMER_ID (must exist): "); int med=promptInt("MEDICATION_ID (must exist): ");
    if(!existsById(db,"Customers","CUSTOMER_ID",cust)){ cout<<"Customer not found.\n"; return; }
    if(!existsById(db,"Medications","MEDICATION_ID",med)){ cout<<"Medication not found.\n"; return; }
    string doc=prompt("PRESCRIBER_NAME: "); string date=prompt("DATE_PRESCRIBED (YYYY-MM-DD): ");
    int qty=promptInt("QUANTITY: "); int refAllow=promptInt("REFILLS_ALLOWED: "); int refUsed=promptInt("REFILLS_USED: ");
    string dir=prompt("DIRECTIONS: ");
    const char* sql="INSERT INTO Prescriptions (PRESCRIPTION_ID,CUSTOMER_ID,MEDICATION_ID,PRESCRIBER_NAME,DATE_PRESCRIBED,QUANTITY,REFILLS_ALLOWED,REFILLS_USED,DIRECTIONS) VALUES (?,?,?,?,?,?,?,?,?);";
    try{ Stmt s(db,sql);
        sqlite3_bind_int(s.stmt,1,id); sqlite3_bind_int(s.stmt,2,cust); sqlite3_bind_int(s.stmt,3,med);
        sqlite3_bind_text(s.stmt,4,doc.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,5,date.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_int(s.stmt,6,qty); sqlite3_bind_int(s.stmt,7,refAllow); sqlite3_bind_int(s.stmt,8,refUsed);
        sqlite3_bind_text(s.stmt,9,dir.c_str(),-1,SQLITE_TRANSIENT);
        if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        cout<<"Prescription added.\n";
    }catch(const std::exception& e){ cout<<"Error: "<<e.what()<<"\n"; }
}

// UPDATE
void updateCustomerContact(sqlite3* db){
    cout<<"\n== Update Customer (phone/email) ==\n";
    int id=promptInt("CUSTOMER_ID: ");
    if(!existsById(db,"Customers","CUSTOMER_ID",id)){ cout<<"Customer not found.\n"; return; }
    string phone=prompt("New PHONE: "); string email=prompt("New EMAIL: ");
    const char* sql="UPDATE Customers SET PHONE=?, EMAIL=? WHERE CUSTOMER_ID=?;";
    try{ Stmt s(db,sql);
        sqlite3_bind_text(s.stmt,1,phone.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(s.stmt,2,email.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_int(s.stmt,3,id);
        if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        cout<<"Customer updated.\n";
    }catch(const std::exception& e){ cout<<"Error: "<<e.what()<<"\n"; }
}

void updateMedicationPrice(sqlite3* db){
    cout<<"\n== Update Medication Price ==\n";
    int id=promptInt("MEDICATION_ID: ");
    if(!existsById(db,"Medications","MEDICATION_ID",id)){ cout<<"Medication not found.\n"; return; }
    double price=promptDouble("New PRICE (e.g., 12.50): ");
    const char* sql="UPDATE Medications SET PRICE=? WHERE MEDICATION_ID=?;";
    try{ Stmt s(db,sql);
        sqlite3_bind_double(s.stmt,1,price);
        sqlite3_bind_int(s.stmt,2,id);
        if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        cout<<"Medication price updated.\n";
    }catch(const std::exception& e){ cout<<"Error: "<<e.what()<<"\n"; }
}

// DELETE
void deleteOrderItem(sqlite3* db){
    cout<<"\n== Delete OrderItem ==\n";
    int id=promptInt("ORDERITEM_ID to delete: ");
    const char* sql="DELETE FROM OrderItems WHERE ORDERITEM_ID=?;";
    try{ Stmt s(db,sql);
        sqlite3_bind_int(s.stmt,1,id);
        if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        if(sqlite3_changes(db)>0) cout<<"OrderItem deleted.\n"; else cout<<"No row deleted (check ID).\n";
    }catch(const std::exception& e){ cout<<"Error: "<<e.what()<<"\n"; }
}

// TRANSACTION
double getMedPrice(sqlite3* db, int medId){
    const char* sql="SELECT PRICE FROM Medications WHERE MEDICATION_ID=?;";
    Stmt s(db,sql); sqlite3_bind_int(s.stmt,1,medId);
    if(sqlite3_step(s.stmt)==SQLITE_ROW) return sqlite3_column_double(s.stmt,0);
    throw std::runtime_error("Medication not found.");
}

void transactionCreateOrder(sqlite3* db){
    cout<<"\n== Transaction: Create Order ==\n";
    int orderId=promptInt("ORDER_ID: ");
    int custId=promptInt("CUSTOMER_ID (must exist): ");
    if(!existsById(db,"Customers","CUSTOMER_ID",custId)){ cout<<"Customer not found.\n"; return; }
    string date=prompt("ORDER_DATE (YYYY-MM-DD): ");
    string pay =prompt("PAYMENT_METHOD (Cash/Card/Insurance/Other): ");

    char* err=nullptr;
    if(sqlite3_exec(db,"BEGIN TRANSACTION;",nullptr,nullptr,&err)!=SQLITE_OK){
        cout<<"BEGIN failed: "<<(err?err:"")<<"\n"; if(err) sqlite3_free(err); return;
    }
    bool ok=true;
    try{
        // 1) Insert order with total 0
        {
            const char* sql="INSERT INTO Orders (ORDER_ID,CUSTOMER_ID,ORDER_DATE,PAYMENT_METHOD,TOTAL_AMOUNT) VALUES (?,?,?,?,0.0);";
            Stmt s(db,sql);
            sqlite3_bind_int(s.stmt,1,orderId);
            sqlite3_bind_int(s.stmt,2,custId);
            sqlite3_bind_text(s.stmt,3,date.c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_text(s.stmt,4,pay.c_str(),-1,SQLITE_TRANSIENT);
            if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        }
        // 2) Add items
        while(true){
            string more=prompt("Add item? (y/n): "); if(more!="y"&&more!="Y") break;
            int itemId=promptInt("ORDERITEM_ID: ");
            int medId =promptInt("MEDICATION_ID: ");
            int qty   =promptInt("QUANTITY: ");
            if(!existsById(db,"Medications","MEDICATION_ID",medId)) throw std::runtime_error("Medication not found.");
            double unit=getMedPrice(db,medId);
            const char* sql="INSERT INTO OrderItems (ORDERITEM_ID,ORDER_ID,MEDICATION_ID,QUANTITY,UNIT_PRICE) VALUES (?,?,?,?,?);";
            Stmt s(db,sql);
            sqlite3_bind_int(s.stmt,1,itemId);
            sqlite3_bind_int(s.stmt,2,orderId);
            sqlite3_bind_int(s.stmt,3,medId);
            sqlite3_bind_int(s.stmt,4,qty);
            sqlite3_bind_double(s.stmt,5,unit);
            if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
            cout<<"  Added item with unit price $"<<unit<<"\n";
        }
        // 3) Recompute order total
        {
            const char* sql=
                "UPDATE Orders "
                "SET TOTAL_AMOUNT = (SELECT IFNULL(SUM(QUANTITY*UNIT_PRICE),0) FROM OrderItems WHERE ORDER_ID=?) "
                "WHERE ORDER_ID=?;";
            Stmt s(db,sql);
            sqlite3_bind_int(s.stmt,1,orderId);
            sqlite3_bind_int(s.stmt,2,orderId);
            if(sqlite3_step(s.stmt)!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db));
        }
    }catch(const std::exception& e){
        cout<<"Transaction error: "<<e.what()<<"\n"; ok=false;
    }
    if(ok){
        if(sqlite3_exec(db,"COMMIT;",nullptr,nullptr,&err)!=SQLITE_OK){ cout<<"COMMIT failed: "<<(err?err:"")<<"\n"; if(err) sqlite3_free(err); }
        else cout<<"Order committed.\n";
    }else{
        if(sqlite3_exec(db,"ROLLBACK;",nullptr,nullptr,&err)!=SQLITE_OK) cout<<"ROLLBACK failed.\n";
        else cout<<"Rolled back.\n";
        if(err) sqlite3_free(err);
    }
}

// REPORTS 
void reportOrdersWithItems(sqlite3* db){
    cout<<"\n== Report: Orders with Items ==\n";
    const char* sql=
        "SELECT o.ORDER_ID,o.ORDER_DATE,o.PAYMENT_METHOD,"
        "       c.FIRST_NAME||' '||c.LAST_NAME AS Customer,"
        "       m.BRAND_NAME,i.QUANTITY,i.UNIT_PRICE,o.TOTAL_AMOUNT "
        "FROM Orders o "
        "JOIN Customers c ON o.CUSTOMER_ID=c.CUSTOMER_ID "
        "JOIN OrderItems i ON o.ORDER_ID=i.ORDER_ID "
        "JOIN Medications m ON i.MEDICATION_ID=m.MEDICATION_ID "
        "ORDER BY o.ORDER_ID,i.ORDERITEM_ID;";
    try{ Stmt s(db,sql);
        while(sqlite3_step(s.stmt)==SQLITE_ROW){
            int oid=sqlite3_column_int(s.stmt,0);
            const unsigned char* date=sqlite3_column_text(s.stmt,1);
            const unsigned char* pay =sqlite3_column_text(s.stmt,2);
            const unsigned char* cust=sqlite3_column_text(s.stmt,3);
            const unsigned char* med =sqlite3_column_text(s.stmt,4);
            int qty=sqlite3_column_int(s.stmt,5);
            double unit=sqlite3_column_double(s.stmt,6);
            double total=sqlite3_column_double(s.stmt,7);
            cout<<"Order "<<oid<<" | "<<(date?(const char*)date:"")<<" | "<<(pay?(const char*)pay:"")
                <<" | "<<(cust?(const char*)cust:"")
                <<" | Item: "<<(med?(const char*)med:"")<<" x"<<qty<<" @$"<<unit
                <<" | Total: $"<<total<<"\n";
        }
    }catch(const std::exception& e){ cout<<"Report error: "<<e.what()<<"\n"; }
}

void reportPrescriptionsByCustomer(sqlite3* db){
    cout<<"\n== Report: Prescriptions by Customer ==\n";
    const char* sql=
        "SELECT c.CUSTOMER_ID, c.FIRST_NAME||' '||c.LAST_NAME AS Customer, "
        "       p.PRESCRIPTION_ID, p.DATE_PRESCRIBED, p.QUANTITY, "
        "       m.GENERIC_NAME||' ('||m.STRENGTH||' '||m.FORM||')' AS Med "
        "FROM Prescriptions p "
        "JOIN Customers c ON p.CUSTOMER_ID=c.CUSTOMER_ID "
        "JOIN Medications m ON p.MEDICATION_ID=m.MEDICATION_ID "
        "ORDER BY c.CUSTOMER_ID,p.PRESCRIPTION_ID;";
    try{ Stmt s(db,sql);
        while(sqlite3_step(s.stmt)==SQLITE_ROW){
            int cid=sqlite3_column_int(s.stmt,0);
            const unsigned char* cust=sqlite3_column_text(s.stmt,1);
            int rx=sqlite3_column_int(s.stmt,2);
            const unsigned char* date=sqlite3_column_text(s.stmt,3);
            int qty=sqlite3_column_int(s.stmt,4);
            const unsigned char* med=sqlite3_column_text(s.stmt,5);
            cout<<"Customer "<<cid<<" | "<<(cust?(const char*)cust:"")
                <<" | Rx "<<rx<<" | "<<(date?(const char*)date:"")
                <<" | Qty "<<qty<<" | "<<(med?(const char*)med:"")<<"\n";
        }
    }catch(const std::exception& e){ cout<<"Report error: "<<e.what()<<"\n"; }
}

// Menus
void menuAdd(sqlite3* db){
    while(true){
        cout<<"\nAdd Menu\n1) Add Customer\n2) Add Prescription\n0) Back\nChoice: ";
        int ch; if(!(cin>>ch)){ clearInput(); continue; } clearInput();
        if(ch==0) return;
        if(ch==1) addCustomer(db);
        else if(ch==2) addPrescription(db);
        else cout<<"Invalid.\n";
    }
}
void menuUpdate(sqlite3* db){
    while(true){
        cout<<"\nUpdate Menu\n1) Update Customer phone/email\n2) Update Medication price\n0) Back\nChoice: ";
        int ch; if(!(cin>>ch)){ clearInput(); continue; } clearInput();
        if(ch==0) return;
        if(ch==1) updateCustomerContact(db);
        else if(ch==2) updateMedicationPrice(db);
        else cout<<"Invalid.\n";
    }
}
void menuDelete(sqlite3* db){
    while(true){
        cout<<"\nDelete Menu\n1) Delete OrderItem by ID\n0) Back\nChoice: ";
        int ch; if(!(cin>>ch)){ clearInput(); continue; } clearInput();
        if(ch==0) return;
        if(ch==1) deleteOrderItem(db);
        else cout<<"Invalid.\n";
    }
}
void menuReports(sqlite3* db){
    while(true){
        cout<<"\nReports\n1) Orders with Items (JOIN)\n2) Prescriptions by Customer (JOIN)\n0) Back\nChoice: ";
        int ch; if(!(cin>>ch)){ clearInput(); continue; } clearInput();
        if(ch==0) return;
        if(ch==1) reportOrdersWithItems(db);
        else if(ch==2) reportPrescriptionsByCustomer(db);
        else cout<<"Invalid.\n";
    }
}

int main(){
    try{
        
        const char* DB_PATH = "dawnrx.db";
        sqlite3* db=openDb(DB_PATH);
        cout<<"Opened SQLite DB: "<<DB_PATH<<"\n";
        while(true){
            cout<<"\n=== DawnRx Menu ===\n"
                <<"1) Add\n2) Update\n3) Delete\n4) Transaction: Create Order\n5) Reports (JOIN)\n0) Exit\nChoice: ";
            int ch; if(!(cin>>ch)){ clearInput(); continue; } clearInput();
            if(ch==0) break;
            if(ch==1) menuAdd(db);
            else if(ch==2) menuUpdate(db);
            else if(ch==3) menuDelete(db);
            else if(ch==4) transactionCreateOrder(db);
            else if(ch==5) menuReports(db);
            else cout<<"Invalid.\n";
        }
        sqlite3_close(db);
        cout<<"Goodbye!\n";
        return 0;
    }catch(const std::exception& e){
        cout<<"Fatal error: "<<e.what()<<"\n"; return 1;
    }
}