<?php

// Applications can call this file with a username and password to check if the password is correct.
// They should expect a string response of either `true` or `false`.

// Two inputs are required when calling this file.
$input_username = $argv[1];
$input_password = $argv[2];

// Composr defaults to a ratchet of 10, but this can be changed if Composr was configured differently.
$ratchet = 10;

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

// Get the correct salt and pass_hash_salted from the database.
try {
 $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password, $pdo_options);
 $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
 $stmt = $conn->prepare("SELECT m_pass_hash_salted, m_pass_salt FROM cms_f_members WHERE m_username = :input_username");
 $stmt->bindParam(":input_username", $input_username, PDO::PARAM_STR);
 if($stmt->execute()) {
  if($stmt->rowCount() == 1){
   if($row = $stmt->fetch()){
    $pass_hash_salted = $row["m_pass_hash_salted"];
    $salt = $row["m_pass_salt"];
   }
  }
 }
} catch(PDOException $e) {
 echo "Error: " . $e->getMessage() . "\n";
 echo $e->getMessage() . $e->getPrevious()->getMessage() . PHP_EOL;
}
$conn = null;

// Use the salt to calculate a pass_hash_salted, and compare it to the one from the database.
if (password_verify($salt . md5($input_password), $pass_hash_salted))
{
 echo "true";
} else {
 echo "false";
}

?>
