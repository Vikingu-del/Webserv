import requests

def post_content():
    content = input("Enter the content to post: ")
    response = requests.post("http://yourwebsite.com/post", data={"content": content})
    if response.status_code == 200:
        print("Content posted.")
    else:
        print("Failed to post content.")

def delete_content():
    content_id = input("Enter the content ID to delete: ")
    response = requests.post("http://yourwebsite.com/delete", data={"id": content_id})
    if response.status_code == 200:
        print("File deleted.")
    else:
        print("Failed to delete content.")

def main():
    choice = input("Do you want to post content or delete content? (post/delete): ")
    if choice.lower() == "post":
        post_content()
    elif choice.lower() == "delete":
        delete_content()
    else:
        print("Invalid choice.")

if __name__ == "__main__":
    main()