# @TODO: implement client


#!/usr/bin/env python3
import requests
import json
import time


class TaskClient:
    def __init__(self, base_url="http://localhost:5000"):
        self.base_url = base_url
        self.session = requests.Session()

    def create_task(self, session_id, cmd, args=""):
        """Create a new task"""
        endpoint = f"{self.base_url}/admin/session/task/create"
        payload = {"session_id": session_id, "cmd": cmd, "args": args}

        response = self.session.post(endpoint, json=payload)
        return response.json(), response.status_code

    def update_task_status(self, task_id, status, result=""):
        """Update a task's status"""
        endpoint = f"{self.base_url}/admin/session/task/update"
        payload = {"task_id": task_id, "status": status, "result": result}

        response = self.session.post(endpoint, json=payload)
        return response.json(), response.status_code

    def list_active_tasks(self, session_id):
        """List all active tasks for a session"""
        endpoint = f"{self.base_url}/admin/session/task/list"
        payload = {"session_id": session_id}

        response = self.session.get(endpoint, json=payload)
        return response.json(), response.status_code


def make_tasks(client):
    tasks = []
    for i in range(3):
        task_name = f"test_task_{i}"
        task_json, status = client.create_task(
            session_id=SESSION_ID,
            cmd="shell",
            args="",
        )

        if status == 201:
            tasks.append(task_json)
            print(f"✅ Created task: {task_name} (ID: {task_json['id']})")
        else:
            print(f"❌ Failed to create task: {task_name} - Status: {status}")


# Test the client
if __name__ == "__main__":
    # Configuration
    BASE_URL = "http://localhost:5000"  # Change this to match your server
    SESSION_ID = 1

    # Create client
    client = TaskClient(BASE_URL)
    make_tasks(client)
    # Test case: List active tasks
    print("\n2. Listing active tasks...")
    active_tasks, status = client.list_active_tasks(SESSION_ID)

    if status == 200:
        print(f"✅ Retrieved {len(active_tasks)} active tasks:")
        for task in active_tasks:
            print(
                f"   - Task ID: {task['id']}, Status: {task['status']}, Cmd: {task['cmd']}"
            )
    else:
        print(f"❌ Failed to retrieve active tasks - Status: {status}")
