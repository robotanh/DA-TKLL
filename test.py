import pyrebase

# Your Firebase config
config = {
  "apiKey": "AIzaSyADY4l9sDZnqcR48svSYRanIiVyZ_IQJCk",
  "authDomain": "esp32-project-d31cd.firebaseapp.com",
  "databaseURL": "https://esp32-project-d31cd-default-rtdb.asia-southeast1.firebasedatabase.app",
  "projectId": "esp32-project-d31cd",
  "storageBucket": "esp32-project-d31cd.appspot.com",
}
firebase = pyrebase.initialize_app(config)

auth = firebase.auth()

# Log the user in (you may need to authenticate the user first)
user = auth.sign_in_anonymous()

# Get a reference to the database service
db = firebase.database()

user_uid = "-Ni8LyV12uM-mJa_zP50"

new_data_LED = {"analog": 200, "digital": True}
new_data = {"name": "Tuan em", "age": 20}
# Update the data for the specific user
db.child("users").child(user_uid).update(new_data, user['idToken'])

print("User data updated successfully")
