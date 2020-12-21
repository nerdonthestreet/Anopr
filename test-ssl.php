<?php

// How to use this file:
/* 
 To check that SSL/TLS is being used when accessing the MySQL server,
 remove the $pdo_options variable from the $conn and observe that
 no SSL cipher is in use, then add $pdo_options back to the $conn
 and observe that an SSL cipher is in use.
*/

// One input is required when calling this file.
$input_username = $argv[1];

// Configure a MySQL/MariaDB account with read access to Composr's cms_f_members table here.
$servername = "";
$username = "";
$password = "";
$dbname = "";

// Connect via SSL/TLS.
$pdo_options = [
 PDO::MYSQL_ATTR_SSL_CA => '',
 PDO::MYSQL_ATTR_SSL_CERT => '',
 PDO::MYSQL_ATTR_SSL_KEY => '',
];

// Get the email address of the Composr user with the provided username.
try {
  $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password, $pdo_options);
  $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
  echo "Connected successfully"; 
  var_dump($conn->query("SHOW STATUS LIKE 'Ssl_cipher';")->fetchAll());
  $conn = null;
} catch(PDOException $e) {
  echo "Error: " . $e->getMessage();
}
$conn = null;

// Return the email address, or a placeholder if one was not found.
if (!empty($email_address)) {
  echo $email_address;
} else {
  echo "invalidircemail@example.com";
}

?>
