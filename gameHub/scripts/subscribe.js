// Handle Email Subscription Form Submission
document.getElementById('subscribeForm').addEventListener('submit', function(event) {
    event.preventDefault();
    const formData = new FormData(event.target);

    fetch('/cgi-bin/form.py', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        alert('Subscription successful!');
        console.log(data);
    })
    .catch(error => console.error('Error:', error));
});

// Handle File Upload via Drag-and-Drop
const dropZone = document.getElementById('dropZone');
const postContent = document.getElementById('postContent');

dropZone.addEventListener('dragover', (event) => {
    event.preventDefault();
    dropZone.classList.add('dragging');
});

dropZone.addEventListener('dragleave', () => {
    dropZone.classList.remove('dragging');
});

dropZone.addEventListener('drop', (event) => {
    event.preventDefault();
    dropZone.classList.remove('dragging');
    const files = event.dataTransfer.files;
    if (files.length > 0) {
        handleFileUpload(files[0]);
    }
});

dropZone.addEventListener('click', () => {
    const fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.onchange = () => {
        if (fileInput.files.length > 0) {
            handleFileUpload(fileInput.files[0]);
        }
    };
    fileInput.click();
});

function handleFileUpload(file) {
    const reader = new FileReader();
    reader.onload = (event) => {
        postContent.value = event.target.result;
        postContent.removeAttribute('hidden');
    };
    reader.readAsText(file);
}

// Handle Content Post Form Submission
document.getElementById('postForm').addEventListener('submit', function(event) {
    event.preventDefault();
    const formData = new FormData(event.target);

    fetch('/cgi-bin/form.py', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        alert('Post successful!');
        console.log(data);
    })
    .catch(error => console.error('Error:', error));
});

// Handle Content Delete Form Submission
document.getElementById('deleteForm').addEventListener('submit', function(event) {
    event.preventDefault();
    const formData = new FormData(event.target);

    fetch('/cgi-bin/form.py', {
        method: 'DELETE',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        alert('Deletion successful!');
        console.log(data);
    })
    .catch(error => console.error('Error:', error));
});
