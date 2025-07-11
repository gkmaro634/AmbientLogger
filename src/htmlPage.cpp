const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>WiFi設定</title></head>
<body>
<h2>WiFi 設定</h2>
<form action="/save" method="POST" autocomplete="off">
SSID: <input type="text" name="ssid" required><br>
Password: <input type="password" name="password" required><br>
<input type="submit" value="保存">
</form>
</body>
</html>
)rawliteral";