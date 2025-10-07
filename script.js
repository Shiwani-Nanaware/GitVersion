// --- Global Data Store (Simulates a backend) ---
let repositories = [
    {
        name: 'LeetCode',
        description: 'DSA Questions',
        createdDate: '10/4/2025',
        isPrivate: false,
        files: [
            {
                name: 'LeetCodeSolutions.js',
                info: 'Added sample solution file',
                date: 'a few seconds ago'
            }
        ],
        commits: [
            {
                message: 'First commit: Added LeetCodeSolutions.js',
                author: 'Shiwani',
                date: 'October 7, 2025'
            }
        ]
    }
];
let tasks = [];
let currentRepoName = '';
let currentFileName = '';
let undoStack = [];
let redoStack = [
    { action: 'Add', description: 'Hello', date: '10/7/2025, 5:08:00 PM' }
];

// --- Dashboard Functions ---
function renderDashboard() {
    const repoGrid = document.getElementById('repo-grid');
    repoGrid.innerHTML = '';
    repositories.forEach(repo => {
        const repoCard = document.createElement('div');
        repoCard.className = 'repo-card';
        repoCard.innerHTML = `
            <div class="repo-card-header">
                <h3 onclick="goToRepoPage('${repo.name}')">${repo.name}</h3>
                <div class="repo-actions-icons">
                    <button class="repo-card-edit-btn" onclick="openEditRepoModal('${repo.name}')">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M21.731 2.269a2.27 2.27 0 0 0-3.22-.001l-14 14a2.27 2.27 0 0 0 0 3.22L7.545 22H2v-5.545l14-14a2.27 2.27 0 0 0 3.22 0zM17.06 4.94l-11.85 11.85a.77.77 0 0 0 0 1.09L13.09 20h-8.09a.5.5 0 0 1-.5-.5v-8.09l11.85-11.85a.77.77 0 0 1 1.09 0z"/>
                        </svg>
                    </button>
                    <button class="repo-card-delete-btn" onclick="openDeleteRepoModal('${repo.name}')">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/>
                        </svg>
                    </button>
                </div>
            </div>
            <p>${repo.description}</p>
            <span>Created ${repo.createdDate}</span>
        `;
        repoGrid.appendChild(repoCard);
    });
}

function goToRepoPage(repoName) {
    window.location.href = `repo.html?repo=${encodeURIComponent(repoName)}`;
}

// --- Repository Page Functions ---
function renderRepoPageContent() {
    const params = new URLSearchParams(window.location.search);
    currentRepoName = params.get('repo');
    if (currentRepoName) {
        document.querySelector('.repo-name-text').textContent = decodeURIComponent(currentRepoName);
        document.querySelector('.repo-description-text').textContent = repositories.find(r => r.name === currentRepoName).description;
        showSection('files');
    }
}

function showSection(sectionId) {
    document.querySelectorAll('.content-section').forEach(section => {
        section.style.display = 'none';
    });
    document.querySelectorAll('.nav-btn').forEach(btn => {
        btn.classList.remove('active');
    });

    document.getElementById(`${sectionId}-section`).style.display = 'block';
    document.querySelector(`.nav-btn[onclick="showSection('${sectionId}')"]`).classList.add('active');

    const currentRepo = repositories.find(r => r.name === currentRepoName);
    if (currentRepo) {
        if (sectionId === 'files') {
            renderFiles(currentRepo.files);
        } else if (sectionId === 'commits') {
            renderCommits(currentRepo.commits);
        }
    }
}

function renderFiles(files) {
    const filesContainer = document.getElementById('files-section');
    let filesList = filesContainer.querySelector('.file-list-container');
    if (!filesList) {
        filesContainer.innerHTML += '<div class="file-list-container"></div>';
        filesList = filesContainer.querySelector('.file-list-container');
    }
    filesList.innerHTML = '';
    
    if (files.length === 0) {
        filesList.innerHTML = `
            <div class="no-files-message">
                <img src="no-files-icon.png" alt="No Files Icon">
                <p>No files yet</p>
                <p>Create or upload files to get started</p>
            </div>
        `;
    } else {
        files.forEach(file => {
            const fileItem = document.createElement('div');
            fileItem.className = 'file-item';
            fileItem.innerHTML = `
                <div class="file-details">
                    <span class="file-icon">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M14 2H6c-1.1 0-1.99.9-1.99 2L4 20c0 1.1.89 2 1.99 2H18c1.1 0 2-.9 2-2V8l-6-6zM6 20V4h7v5h5v11H6z"/>
                        </svg>
                    </span>
                    <div class="file-info-text">
                        <span class="file-name">${file.name}</span>
                        <span class="file-meta">${file.info}</span>
                    </div>
                </div>
                <div class="file-actions-icons">
                    <button class="edit-file-btn" onclick="openEditFileModal('${file.name}')">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M19.73 4.931l-1.66-1.66a1.5 1.5 0 0 0-2.12 0l-12 12a1.5 1.5 0 0 0 0 2.12l1.66 1.66a1.5 1.5 0 0 0 2.12 0l12-12a1.5 1.5 0 0 0 0-2.12zM17.41 6.04l-10.4 10.4-1.12-1.12 10.4-10.4 1.12 1.12zM21 5.06l-1.42 1.42-2.12-2.12L18.88 2.94a1 1 0 0 1 1.41 0l.71.71a1 1 0 0 1 0 1.41z"/>
                        </svg>
                    </button>
                    <button class="delete-file-btn" onclick="openDeleteFileModal('${file.name}')">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/>
                        </svg>
                    </button>
                </div>
            `;
            filesList.appendChild(fileItem);
        });
    }
}

function renderCommits(commits) {
    const commitsList = document.getElementById('commits-section').querySelector('.commit-list-container');
    commitsList.innerHTML = '';
    
    if (commits.length === 0) {
        commitsList.innerHTML = '<p class="no-commits-message">No commits yet. Create files to see commits here.</p>';
    } else {
        commits.forEach(commit => {
            const commitItem = document.createElement('div');
            commitItem.className = 'commit-item';
            commitItem.innerHTML = `
                <div class="commit-msg">${commit.message}</div>
                <div class="commit-details">
                    <span class="commit-author">By ${commit.author}</span>
                    <span class="commit-date">on ${commit.date}</span>
                </div>
            `;
            commitsList.appendChild(commitItem);
        });
    }
}

// --- Modal Functions (unchanged from previous response) ---
function openModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.style.display = 'flex';
        document.body.classList.add('modal-open');
        if (modalId === 'taskQueueModal') {
            renderTasks();
        } else if (modalId === 'historyModal') {
            renderHistory();
        }
    }
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.style.display = 'none';
        document.body.classList.remove('modal-open');
    }
}

window.onclick = function(event) {
    const modals = document.querySelectorAll('.modal');
    modals.forEach(modal => {
        if (event.target == modal) {
            closeModal(modal.id);
        }
    });
}

function openEditRepoModal(repoName) {
    const currentRepo = repositories.find(r => r.name === repoName);
    if (currentRepo) {
        document.getElementById('editRepoName').value = currentRepo.name;
        document.getElementById('editRepoDescription').value = currentRepo.description;
        currentRepoName = repoName;
        openModal('editRepoModal');
    }
}

function openDeleteRepoModal(repoName) {
    document.getElementById('deleteRepoName').textContent = repoName;
    currentRepoName = repoName;
    openModal('deleteRepoModal');
}

function openEditFileModal(fileName) {
    document.getElementById('editFileName').value = fileName;
    currentFileName = fileName;
    openModal('editFileModal');
}

function openDeleteFileModal(fileName) {
    document.getElementById('deleteFileName').textContent = fileName;
    currentFileName = fileName;
    openModal('deleteFileModal');
}

// --- Modal Action Handlers (Placeholders) ---
function createRepository() { /* ... */ }
function saveRepoChanges() { /* ... */ }
function confirmDeleteRepo() { /* ... */ }
function saveFileChanges() { /* ... */ }
function confirmDeleteFile() { /* ... */ }
function createNewFile() { /* ... */ }
function uploadFile() { /* ... */ }
function updateFileName(input) { /* ... */ }
function addTask() { /* ... */ }
function removeTask(id) { /* ... */ }
function renderTasks() { /* ... */ }
function viewHistory() { /* ... */ }


// --- Page Load & Initialization ---
document.addEventListener('DOMContentLoaded', () => {
    if (window.location.pathname.includes('repo.html')) {
        renderRepoPageContent();
    } else {
        renderDashboard();
    }
});