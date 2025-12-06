📌 Student Ticketing / Helpdesk System (C Project)

A simple Queue-based Ticket Management System built in C, designed for student and admin use. It includes a login system, registration, file handling, and full ticket CRUD features using a manually implemented linked-list queue.

🚀 Features
🔐 User System

Register new Admin/Student users

Login with password masking

Stores credentials in credentials.txt

🎫 Ticketing System (Queue)

Students can:

Raise a ticket (enqueue)

View their tickets

Check status

Admins can:

View all tickets

Resolve (dequeue)

Update ticket status

🧰 Queue Operations Used

enqueueTicket() – Add a new ticket

dequeueTicket() – Resolve oldest ticket

peekTicket() – Check next pending

updateTicket() – Modify ticket status

📁 Project Structure
main.c        → Login + menus  
ticket.c      → Queue + ticket logic  
ticket.h      → Structs + prototypes  
credentials.txt (auto)  
tickets.txt      (auto)
README.md

🛠 How to Compile (VS Code / GCC)

Compile:

gcc main.c ticket.c -o ticket


Run:

./ticket

📄 Default Admin

If no credential file exists, the system auto-creates:

Username: admin
Password: password
Role: admin

🧵 Queue Explanation

Tickets are stored as linked list nodes:

New tickets → added at rear

Resolved tickets → removed from front

This ensures FIFO behavior.
