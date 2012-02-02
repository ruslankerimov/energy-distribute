$(function() {
alert(1);
    $.ajax({
        url : '../output/' + BUILD_ID + '/report.js',
        isLocal : true,
        success : function() {
            alert(2);
        }
    })
});
