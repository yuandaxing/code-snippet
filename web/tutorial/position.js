var x=100,y=150;
function pos(dx,dy) {
    if (!document.getElementById) return;
    x += 100*dx;
    y += 100*dy;
    obj = document.getElementById("square");
    obj.style.top=y + "px";
    obj.style.left=x + "px";
}
function hideSquare() {
    if (!document.getElementById) return;
    obj = document.getElementById("square");
    obj.style.display="none";
}
function showSquare() {
    if (!document.getElementById) return;
    obj = document.getElementById("square");
    obj.style.display="block";
}
