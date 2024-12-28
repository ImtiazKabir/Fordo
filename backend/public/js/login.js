document.addEventListener("DOMContentLoaded", () => {
    const form = document.getElementById("login-form");

    // Redirect logged-in users
    const userToken = localStorage.getItem("user_token");
    if (userToken) {
        window.location.href = "todo.html";
    }

    form.addEventListener("submit", async (e) => {
        e.preventDefault();

        const username = document.getElementById("username").value;
        const password = document.getElementById("password").value;
        const hashedPassword = btoa(password); // Simple base64 hash for demonstration

        try {
            const response = await fetch("/api/login", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
                body: JSON.stringify({ username, password: hashedPassword }),
            });

            const result = await response.json();

            if (result.status === "success") {
                localStorage.setItem("user_token", result.data.user_token);
                window.location.href = "todo.html";
            } else {
                alert(result.message.join(", "));
            }
        } catch (error) {
            console.error("Error during login:", error);
            alert("Failed to login. Please try again later.");
        }
    });
});

