document.addEventListener("DOMContentLoaded", () => {
    const signupForm = document.getElementById("signup-form");

    // Redirect logged-in users to the index page
    const userToken = localStorage.getItem("user_token");
    if (userToken) {
        window.location.href = "todo.html";
    }

    signupForm.addEventListener("submit", async (event) => {
        event.preventDefault();

        const username = document.getElementById("username").value.trim();
        const password = document.getElementById("password").value;

        // Simple validation
        if (!username || !password) {
            alert("Both fields are required.");
            return;
        }

        // Hash the password (you can use a library like SHA256 for this, but keeping it simple here)
        const hashedPassword = btoa(password); // Basic encoding (use a proper hashing library in production)

        try {
            const response = await fetch("/api/signup", {
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
            console.error("Signup failed:", error);
            alert("An error occurred during signup. Please try again.");
        }
    });
});

