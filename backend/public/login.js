async function hashPassword(password) {
  const encoder = new TextEncoder();
  const data = encoder.encode(password);
  const hashBuffer = await crypto.subtle.digest('SHA-256', data);
  const hashArray = Array.from(new Uint8Array(hashBuffer));
  return hashArray.map(byte => byte.toString(16).padStart(2, '0')).join('');
}

document.getElementById('login-form').addEventListener('submit', async (event) => {
  event.preventDefault(); // Prevent the form from refreshing the page
  
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;

  try {
    const hashedPassword = await hashPassword(password);
    const data = { username, password: hashedPassword };

    const response = await fetch('http://localhost:3000/api/login', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });

    console.log(response)
    const result = await response.json();
    console.log('Response:', result);
  } catch (error) {
    console.error('Error:', error);
  }
});

