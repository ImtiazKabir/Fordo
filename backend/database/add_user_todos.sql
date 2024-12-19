INSERT INTO todos (user_id, text, is_done)
SELECT id, ?, 0
FROM users
WHERE username = ?;
