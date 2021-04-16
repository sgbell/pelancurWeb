function secondsToHMS(s){
    return application.secondsToHMS(s);
}

var taskbarhtml = '';
var application = (function(app){
    app.currentScreen=undefined;
    app.refreshId=undefined;

    app.initialize = function(){
        $(document).ajaxError(function(event, request, settings){
            $('.main-window').append('<p>Error requesting :'+settings.url+'</p>');
            $('.main-window').append('<p>Request'+request+'</p>');
        });
        left = ($('body').width()/2)-($('body img').width()/2)       
        $('div.loading img').css('left',left);

        $('#navigation li label').on('click', function(){
            if ($(this).find('a').length!=0) return;
            if ($(this).next().prop('tagName')=='UL'){
                subMenu = $(this).next();
     
                $(this).closest('ul').find('ul:visible').each(function(index,element){
                    if ($(element)!=$(subMenu)){
                        $(element).addClass('hidden');
                    }
                });
                $(subMenu).removeClass('hidden');
            }
        });

        $(document.body).on('click','[pw-action=exec]', function(event){
            element = $(this);
            event.preventDefault();
            $.get($(this).prop('href'), {}, function(data){
                console.log(data);
            });
        });

        app.startStatusRefresh();
    };

    app.startStatusRefresh=function(){ 
        app.refreshId=setInterval(function(){
            $.get('/activity/', {}, function(data){
                if (data!=taskbarhtml){
                    $('div.main-window').html(data);
                    taskbarhtml=data;
                    // iterate over tasks and add tooltip with full name
                    // add on click to each task to active the action menu
                    $('div.main-window .details').on('click', function(event){
                        element = $(this);
                        $('div.main-window .task .taskMenu').each(function(index,taskMenu){
                            if ($(taskMenu).data('pid')!=$(element).find('ul.taskMenu').data('pid')){
                                $(taskMenu).addClass('hidden');
                            }
                        });
                        if ($(element).closest('.task').find('ul.taskMenu').hasClass('hidden')){
                            $(element).closest('.task').find('ul.taskMenu').removeClass('hidden').addClass('open');
                        } else {
                            $(element).closest('.task').find('ul.taskMenu').addClass('hidden').removeClass('open');
                        }
                    });
                }
            }).fail(function(){
                clearInterval(app.refreshId);
            });
        },3000);
    };

    return app;
})({});

$(document).ready(function(){
    application.initialize();
});
