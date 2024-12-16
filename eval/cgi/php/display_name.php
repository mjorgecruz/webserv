<!DOCTYPE html>
<html>
<head>
    <title>Display Name</title>
</head>
<body>
    <h1>Display Name</h1>
    <form method="post" action="display_name.php">
        <label for="name">Enter your name:</label>
        <input type="text" id="name" name="name">
        <input type="submit" value="Submit">
    </form>
    <?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $name = htmlspecialchars($_POST["name"]);
        if (!empty($name)) {
            echo "<h2>Your name is: $name</h2>";
        } else {
            echo "<h2>No name provided</h2>";
        }
    }
    ?>
</body>
</html>