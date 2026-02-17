-- SQLite
-- Create Customers table
CREATE TABLE Customers (
    CUSTOMER_ID INTEGER PRIMARY KEY,
    FIRST_NAME VARCHAR(50),
    LAST_NAME VARCHAR(50),
    DOB DATE,
    PHONE VARCHAR(20),
    EMAIL VARCHAR(100),
    ADDRESS VARCHAR(150)
);

-- Create Medications table
CREATE TABLE Medications (
    MEDICATION_ID INTEGER PRIMARY KEY,
    BRAND_NAME VARCHAR(100),
    GENERIC_NAME VARCHAR(100),
    STRENGTH VARCHAR(50),
    FORM VARCHAR(50),
    PRICE DECIMAL(10, 2)
);

-- Create Prescriptions table
CREATE TABLE Prescriptions (
    PRESCRIPTION_ID INTEGER PRIMARY KEY,
    CUSTOMER_ID INTEGER,
    MEDICATION_ID INTEGER,
    PRESCRIBER_NAME VARCHAR(100),
    DATE_PRESCRIBED DATE,
    QUANTITY INT,
    REFILLS_ALLOWED INT,
    REFILLS_USED INT,
    DIRECTIONS VARCHAR(255),
    FOREIGN KEY (CUSTOMER_ID) REFERENCES Customers(CUSTOMER_ID),
    FOREIGN KEY (MEDICATION_ID) REFERENCES Medications(MEDICATION_ID)
);

-- Create Orders table
CREATE TABLE Orders (
    ORDER_ID INTEGER PRIMARY KEY,
    CUSTOMER_ID INTEGER,
    ORDER_DATE DATE,
    PAYMENT_METHOD VARCHAR(20),
    TOTAL_AMOUNT DECIMAL(10, 2),
    FOREIGN KEY (CUSTOMER_ID) REFERENCES Customers(CUSTOMER_ID)
);

-- Create OrderItems table
CREATE TABLE OrderItems (
    ORDERITEM_ID INTEGER PRIMARY KEY,
    ORDER_ID INTEGER,
    MEDICATION_ID INTEGER,
    QUANTITY INT,
    UNIT_PRICE DECIMAL(10, 2),
    FOREIGN KEY (ORDER_ID) REFERENCES Orders(ORDER_ID),
    FOREIGN KEY (MEDICATION_ID) REFERENCES Medications(MEDICATION_ID)
);

-- Insert records into Customers table
INSERT INTO Customers (FIRST_NAME, LAST_NAME, DOB, PHONE, EMAIL, ADDRESS)
VALUES 
('John', 'Doe', '1990-05-15', '123-456-7890', 'johndoe@example.com', '123 Main St'),
('Jane', 'Smith', '1985-08-20', '234-567-8901', 'janesmith@example.com', '456 Oak St'),
('Alice', 'Johnson', '1992-11-05', '345-678-9012', 'alicej@example.com', '789 Pine St'),
('Bob', 'Brown', '1978-03-22', '456-789-0123', 'bobbrown@example.com', '101 Maple St'),
('Charlie', 'Davis', '2000-01-01', '567-890-1234', 'charlied@example.com', '202 Birch St');

-- Insert records into Medications table
INSERT INTO Medications (BRAND_NAME, GENERIC_NAME, STRENGTH, FORM, PRICE)
VALUES 
('Tylenol', 'Acetaminophen', '500mg', 'Tablet', 5.99),
('Advil', 'Ibuprofen', '200mg', 'Tablet', 8.99),
('Cipro', 'Ciprofloxacin', '250mg', 'Tablet', 15.49),
('Zithromax', 'Azithromycin', '500mg', 'Tablet', 12.99),
('Lipitor', 'Atorvastatin', '10mg', 'Tablet', 20.00);

-- Insert records into Prescriptions table
INSERT INTO Prescriptions (CUSTOMER_ID, MEDICATION_ID, PRESCRIBER_NAME, DATE_PRESCRIBED, QUANTITY, REFILLS_ALLOWED, REFILLS_USED, DIRECTIONS)
VALUES 
(1, 1, 'Dr. Smith', '2023-01-10', 30, 2, 0, 'Take one tablet every 4 hours'),
(2, 2, 'Dr. Lee', '2023-02-12', 20, 1, 0, 'Take one tablet every 6 hours'),
(3, 3, 'Dr. White', '2023-03-05', 10, 0, 0, 'Take one tablet every 12 hours'),
(4, 4, 'Dr. Brown', '2023-04-15', 5, 1, 1, 'Take one tablet daily for 5 days'),
(5, 5, 'Dr. Green', '2023-05-20', 40, 3, 1, 'Take one tablet every night');

-- Insert records into Orders table
INSERT INTO Orders (CUSTOMER_ID, ORDER_DATE, PAYMENT_METHOD, TOTAL_AMOUNT)
VALUES 
(1, '2023-06-01', 'Credit Card', 25.50),
(2, '2023-06-03', 'Cash', 50.00),
(3, '2023-06-05', 'Debit Card', 75.20),
(4, '2023-06-07', 'Credit Card', 30.00),
(5, '2023-06-09', 'Cash', 100.00);

-- Insert records into OrderItems table
INSERT INTO OrderItems (ORDER_ID, MEDICATION_ID, QUANTITY, UNIT_PRICE)
VALUES 
(1, 1, 2, 5.99),
(2, 2, 3, 8.99),
(3, 3, 1, 15.49),
(4, 4, 1, 12.99),
(5, 5, 4, 20.00);