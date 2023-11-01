import pyrebase
config = {
  "apiKey": "AIzaSyADY4l9sDZnqcR48svSYRanIiVyZ_IQJCk",
  "authDomain": "esp32-project-d31cd.firebaseapp.com",
  "databaseURL": "https://esp32-project-d31cd-default-rtdb.asia-southeast1.firebasedatabase.app",
  "projectId": "esp32-project-d31cd",
  "storageBucket": "esp32-project-d31cd.appspot.com",
}
firebase = pyrebase.initialize_app(config)

auth = firebase.auth()

# Log the user in
user = auth.sign_in_anonymous()

# Get a reference to the database service
db = firebase.database()

# Use the reference to get/set data
data = {"name": "John Doe", "age": 30}
db.child("users").push(data, user['idToken'])