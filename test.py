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

# Log in
user = auth.sign_in_anonymous()

# Get a reference to the database service
db = firebase.database()

# Test push data up to firebase
data = {"name": "Tuan Anh", "age": 69}
db.child("users").push(data, user['idToken'])
