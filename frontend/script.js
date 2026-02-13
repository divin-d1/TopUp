// Connect to WebSocket
let ws = new WebSocket("ws://157.173.101.159:8001/ws");

const balancesTable = document.querySelector("#balances tbody");
const balanceData = {};  // Store latest balances by UID

// Handle incoming WebSocket messages
ws.onmessage = (event) => {
    try {
        const msg = JSON.parse(event.data);
        const data = msg.data;

        // Only process if UID exists and a balance field is present
        if (data.uid && (data.new_balance !== undefined || data.balance !== undefined)) {
            const newBalance = data.new_balance ?? data.balance;
            const oldBalance = balanceData[data.uid];
            balanceData[data.uid] = newBalance;

            renderBalances(data.uid, oldBalance !== newBalance);
        }
    } catch (err) {
        console.error("Invalid WebSocket message:", err, event.data);
    }
};

ws.onopen = () => console.log("WebSocket connected");

// Render balances in table
function renderBalances(highlightUid = null, highlight = false) {
    balancesTable.innerHTML = "";
    for (let uid in balanceData) {
        const tr = document.createElement("tr");
        tr.innerHTML = `<td>${uid}</td><td>${balanceData[uid]}</td>`;

        if (highlight && uid === highlightUid) {
            tr.classList.add("highlight");
            setTimeout(() => tr.classList.remove("highlight"), 1000);
        }

        balancesTable.appendChild(tr);
    }
}

// Handle top-up button click
document.querySelector("#topup-btn").addEventListener("click", async () => {
    const uid = document.querySelector("#uid").value.trim();
    const amount = parseInt(document.querySelector("#amount").value);

    if (!uid || !amount || amount <= 0) {
        alert("Enter valid UID and amount");
        return;
    }

    try {
        const res = await fetch("http://157.173.101.159:8001/topup", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ uid, amount })
        });
        const result = await res.json();
        console.log(result);

        // Update balance immediately if backend returns new_balance
        if (result.new_balance !== undefined) {
            const oldBalance = balanceData[uid];
            balanceData[uid] = result.new_balance;
            renderBalances(uid, oldBalance !== result.new_balance);
        }

        document.querySelector("#uid").value = "";
        document.querySelector("#amount").value = "";
    } catch (err) {
        console.error(err);
        alert("Error sending top-up");
    }
});
