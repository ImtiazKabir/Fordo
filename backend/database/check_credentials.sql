SELECT COUNT(*) AS is_valid
FROM users
WHERE username = ? AND password_hash = ?;
