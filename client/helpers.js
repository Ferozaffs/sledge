function isMobile() {
    return /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent);
}

document.addEventListener('DOMContentLoaded', function() {
    if (isMobile()) {
        document.getElementById('fullScreenImg').style.display = 'block';
    }
});

document.getElementById('fullScreenImg').addEventListener('click', function() {
    var elem = document.documentElement;

    var requestFullScreen = elem.requestFullscreen || elem.mozRequestFullScreen || elem.webkitRequestFullscreen || elem.msRequestFullscreen;
    if (requestFullScreen) {
        requestFullScreen.call(elem).then(() => {
            // Hide the button after entering full-screen mode
            document.getElementById('fullScreenImg').style.display = 'none';
        }).catch(err => {
            console.error(`Error attempting to enable full-screen mode: ${err.message} (${err.name})`);
        });
    }
});

document.addEventListener('fullscreenchange', function() {
    if (!document.fullscreenElement && isMobile()) {
        document.getElementById('fullScreenImg').style.display = 'block';
    }
});

document.addEventListener('webkitfullscreenchange', function() {
    if (!document.webkitFullscreenElement && isMobile()) {
        document.getElementById('fullScreenImg').style.display = 'block';
    }
});

document.addEventListener('mozfullscreenchange', function() {
    if (!document.mozFullScreenElement && isMobile()) {
        document.getElementById('fullScreenImg').style.display = 'block';
    }
});

document.addEventListener('MSFullscreenChange', function() {
    if (!document.msFullscreenElement && isMobile()) {
        document.getElementById('fullScreenImg').style.display = 'block';
    }
});