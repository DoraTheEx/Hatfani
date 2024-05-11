import hashlib
import json
import datetime

USER_PATH = r"C:\Users\Yarin Herman\Desktop\C\nooni\users.txt"
MEMORIES_PATH = r"C:\Users\Yarin Herman\Desktop\C\nooni\memories.txt"


class User:
    def __init__(self, name, username, password_hash, strong_user=False, points=0, memories=[]):
        self.name = name
        self.username = username
        self.password_hash = password_hash
        self.strong_user = strong_user
        self.points = points
        self.memories = memories

class Memory:
    def __init__(self, name, content, creator, date_created):
        self.name = name
        self.content = content
        self.creator = creator
        self.date_created = date_created
        self.taker = None

def read_users_from_file(filename):
    users = []
    with open(filename, 'r') as file:
        for line in file:
            data = line.strip().split(',')
            name, username, password_hash, strong_user, points = data[:5]
            memories = json.loads(','.join(data[5:]))
            user = User(name, username, password_hash, strong_user == 'True', int(points), memories)
            users.append(user)
    return users

def read_memories_from_file(filename):
    memories = []
    with open(filename, 'r') as file:
        for line in file:
            data = json.loads(line)
            memory = Memory(data['name'], data['content'], data['creator'], data['date_created'])
            if 'taker' in data:
                memory.taker = data['taker']
            memories.append(memory)
    return memories

def write_users_to_file(users, filename):
    with open(filename, 'w') as file:
        for user in users:
            user_data = [user.name, user.username, user.password_hash, str(user.strong_user), str(user.points)]
            user_data.append(json.dumps(user.memories))
            file.write(','.join(user_data) + '\n')

def write_memories_to_file(memories, filename):
    with open(filename, 'w') as file:
        for memory in memories:
            memory_data = {'name': memory.name, 'content': memory.content, 'creator': memory.creator, 'date_created': memory.date_created}
            if memory.taker:
                memory_data['taker'] = memory.taker
            file.write(json.dumps(memory_data) + '\n')

def md5_hash(text):
    return hashlib.md5(text.encode()).hexdigest()

def login(users):
    username = input("Enter your username: ")
    password = input("Enter your password: ")
    password_hash = md5_hash(password)
    for user in users:
        if user.username == username and user.password_hash == password_hash:
            return user
    return None

def borrow_memory(user, memories):
    if len(user.memories) >= 2 and user.points >= 2:
        print("You can only borrow 1 memory at a time.")
        return
    available_memories = [memory for memory in memories if memory.taker is None]
    if not available_memories:
        print("No memories available for borrowing.")
        return
    print("Available memories:")
    for i, memory in enumerate(available_memories):
        print(f"{i+1}. {memory.name} - {memory.content}")
    choice = input("Enter the number of the memory you want to borrow: ")
    if not choice.isdigit() or not (1 <= int(choice) <= len(available_memories)):
        print("Invalid input. Please enter a valid number.")
        return
    memory = available_memories[int(choice) - 1]
    memory.taker = user.username
    user.memories.append(memory.name)
    user.points += 1
    print("Memory borrowed successfully.")

def return_memory(user, memories):
    if not user.memories:
        print("You have no memories to return.")
        return
    print("Memories you borrowed:")
    for i, memory_name in enumerate(user.memories):
        print(f"{i+1}. {memory_name}")
    choice = input("Enter the number of the memory you want to return: ")
    if not choice.isdigit() or not (1 <= int(choice) <= len(user.memories)):
        print("Invalid input. Please enter a valid number.")
        return
    memory_name = user.memories.pop(int(choice) - 1)
    for memory in memories:
        if memory.name == memory_name:
            memory.taker = None
            print("Memory returned successfully.")
            write_users_to_file(users, USER_PATH)
            write_memories_to_file(memories, MEMORIES_PATH)
            return
    print("Memory not found.")

def search_memory(memories):
    keyword = input("Enter a keyword to search for: ").lower()
    found = False
    for memory in memories:
        if keyword in memory.name.lower() or keyword in memory.content.lower():
            print(f"Memory Name: {memory.name}")
            print(f"Content: {memory.content}")
            print(f"Creator: {memory.creator}")
            print(f"Date Created: {memory.date_created}")
            if memory.taker:
                print(f"Taker: {memory.taker}")
            print()
            found = True
    if not found:
        print("No memories found matching the keyword.")


def strong_menu(users, memories):
    while True:
        print("\nStrong Menu:")
        print("1. Add User")
        print("2. Report Lost Memory")
        print("3. Add Memory")
        print("4. Block User")
        print("5. Back to Main Menu")
        choice = input("Enter your choice: ")
        if choice == '1':
            add_user(users)
            write_users_to_file(users, USER_PATH)
        elif choice == '2':
            report_lost_memory(users, memories)
        elif choice == '3':
            add_memory(memories)
            write_memories_to_file(memories, MEMORIES_PATH)
        elif choice == '4':
            block_user(users)
            write_users_to_file(users, USER_PATH)
        elif choice == '5':
            break
        else:
            print("Invalid choice. Please try again.")


def print_user_stats(user):
    print("User Stats:")
    print(f"Name: {user.name}")
    print(f"Username: {user.username}")
    print(f"Strong User: {'Yes' if user.strong_user else 'No'}")
    print(f"Points: {user.points}")
    print(f"Memories: {', '.join(user.memories)}")


def add_user(users):
    name = input("Enter user's name: ")
    username = input("Enter username: ")
    password = input("Enter password: ")
    password_hash = md5_hash(password)
    strong_user = input("Is this user a strong user? (1 for Yes, 0 for No): ")
    points = input("Enter points: ")
    user = User(name, username, password_hash, bool(int(strong_user)), int(points))
    users.append(user)
    print("User added successfully.")

def report_lost_memory(users, memories):
    print("Memories currently borrowed:")
    borrowed_memories = []
    for memory in memories:
        if memory.taker:
            print(f"- {memory.name} (Taker: {memory.taker})")
            borrowed_memories.append(memory)
    memory_name = input("Enter the name of the memory you want to report as lost: ")
    found = False
    for memory in borrowed_memories:
        if memory.name == memory_name:
            memory.taker = None
            for user in users:
                if memory_name in user.memories:
                    user.points += 1
                    user.memories.remove(memory_name)
            memories.remove(memory)
            print("Memory reported as lost successfully.")
            write_users_to_file(users, USER_PATH)
            write_memories_to_file(memories, MEMORIES_PATH)
            found = True
            break
    if not found:
        print("Memory not found or already returned.")


def add_memory(memories):
    name = input("Enter memory name: ")
    content = input("Enter memory content: ")
    creator = input("Enter creator's name: ")
    date_created = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    memory = Memory(name, content, creator, date_created)
    memories.append(memory)
    print("Memory added successfully.")

def block_user(users):
    username = input("Enter the username of the user you want to block: ")
    for user in users:
        if user.username == username:
            users.remove(user)
            print("User blocked successfully.")
            return
    print("User not found.")

def main():
    users = read_users_from_file(USER_PATH)
    memories = read_memories_from_file(MEMORIES_PATH)

    while True:
        print("\nMain Menu:")
        print("1. Login")
        print("2. Exit")
        choice = input("Enter your choice: ")
        if choice == '1':
            user = login(users)
            if user:
                print(f"Welcome, {user.name}!")
                while True:
                    if user.strong_user:
                        strong_menu(users, memories)
                    print("\nNormal Menu:")
                    print("1. Borrow Memory")
                    print("2. Return Memory")
                    print("3. Check Available Memories")
                    print("4. Search for Memory")
                    print("5. Check users stats")
                    print("6. Logout")
                    choice = input("Enter your choice: ")
                    if choice == '1':
                        borrow_memory(user, memories)
                    elif choice == '2':
                        return_memory(user, memories)
                    elif choice == '3':
                        print("Available memories:")
                        for i, memory in enumerate(memories):
                            if memory.taker is None:
                                print(f"{i+1}. {memory.name}")
                    elif choice == '4':
                        search_memory(memories)
                    elif choice == '5':
                        print_user_stats(user)
                    elif choice == '6':
                        break
                    else:
                        print("Invalid choice. Please try again.")
        elif choice == '2':
            write_users_to_file(users, USER_PATH)
            write_memories_to_file(memories, MEMORIES_PATH)
            print("Exiting program.")
            break
        else:
            print("Invalid choice. Please try again.")

if __name__ == "__main__":
    main()
