let selected_index;
const container = document.getElementsByClassName('container')[0];
const text = new Array(80).fill(' ');
const re = /^[ -~]$/;
const apiRoot = 'http://home.boelling.ca:25566';

function initLcd() {
    // Create boxes
    const box = container.getElementsByClassName('box')[0];
    for (let i = 0; i < 79; i++) {
        const copy = box.cloneNode(true);
        container.appendChild(copy);
    }

    // Select first box
    selected_index = 0;
    container.children[selected_index].classList.add('selected');
    container.children[selected_index].focus();

    // Add event handlers to boxes
    for (let i = 0; i < 80; i++) {
        const box = container.children[i];
        box.addEventListener('click', () => {
            updateSelected(i);
        });
        box.addEventListener('keydown', (event) => {
            if (re.test(event.key)) {
                text[i] = event.key;
                drawText();
                updateSelected((selected_index + 1) % 80);
                updateOk('&nbsp');
            } else if (event.key === 'Enter') {
                submit()
            }
        });
    }

    // Add backspace handler
    container.addEventListener('keydown', (event) => {
        if (event.key === 'Backspace') {
            text[selected_index] = ' ';
            drawText();
            updateSelected((((selected_index - 1) % 80) + 80) % 80);
        }
    });

    // Add submit handler
    const form = document.getElementById('form');
    form.addEventListener('submit', (event) => {
        event.preventDefault();
        submit();
    });

    const initialText = "Type a message here";
    for (let i = 0; i < initialText.length; i++) {
        text[i] = initialText[i];
    }
    drawText();
}

function submit() {
    updateOk('...');
    fetch(apiRoot + '/message', { method: 'POST', body: text.join('') })
        .then((resp) => {
            if (resp.ok) {
                updateOk('Success');
            } else {
                updateOk('Error ' + resp.status);
            }
        });
}

function updateSelected(new_index) {
    container.children[selected_index].classList.remove('selected');
    selected_index = new_index;
    container.children[selected_index].classList.add('selected');
    container.children[selected_index].focus();
}

function drawText() {
    for (let i = 0; i < 80; i++) {
        const box = container.children[i];
        if (text[i] === ' ') {
            box.innerHTML = '&nbsp';
        } else {
            box.innerHTML = text[i];
        }
    }
}

async function pingServer() {
    const response = await fetch(apiRoot + '/ping');
    const pong = await response.text();
    if (pong !== 'pong') {
        alert('Server is offline');
    }
}

function updateOk(text) {
    const ok = document.getElementById('ok');
    ok.innerHTML = text;
}

initLcd();