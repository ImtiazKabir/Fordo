SELECT todos.id, todos.text, todos.is_done
FROM todos
JOIN users ON todos.user_id = users.id
WHERE users.id = ?
