document.addEventListener("DOMContentLoaded", () => {
    const logoutButton = document.getElementById("logout-button");
    const addTodoForm = document.getElementById("add-todo-form");
    const todoInput = document.getElementById("todo-input");
    const todoList = document.getElementById("todo-list");

    const userToken = localStorage.getItem("user_token");
    if (!userToken) {
        window.location.href = "index.html";
    }

    const fetchTodos = async () => {
        try {
            const response = await fetch("/api/get-todos", {
                method: "GET",
                headers: {
                    Authorization: `Bearer ${userToken}`,
                },
            });
            const data = await response.json();
            if (data.status === "success") {
                renderTodos(data.data.todos);
            } else {
                alert("Failed to fetch todos. Please try again.");
            }
        } catch (error) {
            console.error("Error fetching todos:", error);
        }
    };

    const renderTodos = (todos) => {
        todoList.innerHTML = "";
        todos.forEach((todo) => {
            const listItem = document.createElement("li");
            listItem.className = `todo-item ${todo.is_done ? "done" : ""}`;
            listItem.dataset.id = todo.id;

            const text = document.createElement("span");
            text.textContent = todo.todo_text;

            const containerDiv = document.createElement('div');

            const toggleButton = document.createElement("button");
            toggleButton.textContent = todo.is_done ? "⎌  Undo" : "✓  Done";
            toggleButton.addEventListener("click", () => toggleTodo(todo.id));
            toggleButton.style.marginRight = "1em";

            const deleteButton = document.createElement("button");
            deleteButton.textContent = "🗑 Delete";
            deleteButton.addEventListener("click", () => deleteTodo(todo.id));
            
            containerDiv.appendChild(toggleButton);
            containerDiv.appendChild(deleteButton);

            listItem.append(text, containerDiv);
            todoList.appendChild(listItem);
        });
    };

    const addTodo = async (text) => {
        try {
            const response = await fetch("/api/add-todo", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                    Authorization: `Bearer ${userToken}`,
                },
              body: JSON.stringify({ todo_text: text }),
            });
            const data = await response.json();
            if (data.status === "success") {
                fetchTodos();
            } else {
                alert("Failed to add todo. Please try again.");
            }
        } catch (error) {
            console.error("Error adding todo:", error);
        }
    };

    const toggleTodo = async (id) => {
        try {
            const response = await fetch("/api/toggle-todo", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                    Authorization: `Bearer ${userToken}`,
                },
                body: JSON.stringify({ todo_id: id }),
            });
            const data = await response.json();
            if (data.status === "success") {
                fetchTodos();
            } else {
                alert("Failed to toggle todo. Please try again.");
            }
        } catch (error) {
            console.error("Error toggling todo:", error);
        }
    };

    const deleteTodo = async (id) => {
        try {
            const response = await fetch("/api/delete-todo", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                    Authorization: `Bearer ${userToken}`,
                },
                body: JSON.stringify({ todo_id: id }),
            });
            const data = await response.json();
            if (data.status === "success") {
                fetchTodos();
            } else {
                alert("Failed to delete todo. Please try again.");
            }
        } catch (error) {
            console.error("Error deleting todo:", error);
        }
    };

    addTodoForm.addEventListener("submit", (event) => {
        event.preventDefault();
        const text = todoInput.value.trim();
        if (text) {
            addTodo(text);
            todoInput.value = "";
        }
    });

    logoutButton.addEventListener("click", () => {
        localStorage.removeItem("user_token");
        window.location.href = "index.html";
    });

    fetchTodos();
});

