# RFID Top-Up Dashboard

A **real-time RFID-based Top-Up system** for managing card balances. Users can top-up RFID cards via a web interface, and the dashboard displays live updates using WebSockets.

---

## 🌟 Project Overview

This project allows:

* Each card to have a **unique UID**.
* Users to **top-up** amounts for cards through a simple dashboard.
* **Real-time updates** of balances using WebSockets.
* Seamless communication between **frontend** and **FastAPI backend**.

**Technologies Used:**

* **Backend:** Python, FastAPI, Uvicorn, WebSockets
* **Frontend:** HTML, JavaScript (WebSockets & Fetch API)
* **Server:** Python HTTP server for static frontend hosting

**Data Flow:**

1. Frontend sends top-up requests
2. Backend updates the balance
3. WebSocket pushes the updated balance back to the frontend

---

## ⚡ Prerequisites

* Python 3.10+
* pip
* Git
* Access to a server or local machine for hosting frontend & backend

---

## 🛠 Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/divin-d1/TopUp.git
cd TopUp
```

### 2. Backend Setup

```bash
cd backend
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### 3. Start the Backend Server

```bash
uvicorn main:app --host 0.0.0.0 --port 8001
```

This will run the API and WebSocket server.

### 4. Frontend Setup

```bash
cd ../frontend
# Ensure port 8002 is free
ps aux | grep 8002
kill -9 <PID>  # Kill any process using 8002
```

Start the frontend server:

```bash
nohup python3 -m http.server 8002 > frontend.log 2>&1 &
```

Open in browser:

```
http://<server-ip>:8002
```

---

## 🚀 How to Use

1. Enter the **Card UID** in the input field.
2. Enter the **Top-Up Amount**.
3. Click **Top Up**.
4. Watch the dashboard **update balances in real-time**.

---

## 📝 Notes

* Ensure the **backend server** is running before opening the frontend.
* WebSocket messages **keep the balances live**.
* Ports **8001 (backend)** and **8002 (frontend)** must be free.
* Logs are stored in `frontend.log` and backend logs via `uvicorn` stdout.

---

## 📌 License

This project is for **educational purposes** only. Not intended for production use.