jQuery(document).ready( function($){
	var load_ui_css = false; //load jquery ui css?
	/* Time Entry */
	$('#start-time').each(function(i, el){
		$(el).addClass('em-time-input em-time-start').next('#end-time').addClass('em-time-input em-time-end').parent().addClass('em-time-range');
	});
	if( $(".em-time-input").length > 0 ){
		em_setup_timepicker('body');
	}
	/* Calendar AJAX */
	$('.em-calendar-wrapper a').unbind("click");
	$('.em-calendar-wrapper a').undelegate("click");
	$('.em-calendar-wrapper').delegate('a.em-calnav, a.em-calnav', 'click', function(e){
		e.preventDefault();
		$(this).closest('.em-calendar-wrapper').prepend('<div class="loading" id="em-loading"></div>');
		var url = em_ajaxify($(this).attr('href'));
		$(this).closest('.em-calendar-wrapper').load(url, function(){$(this).trigger('em_calendar_load');});
	} );

	//Events Search
	$(document).delegate('.em-toggle', 'click change', function(e){
		e.preventDefault();
		//show or hide advanced tickets, hidden by default
		var el = $(this);
		var rel = el.attr('rel').split(':');
		if( el.hasClass('show-search') ){
			if( rel.length > 1 ){ el.closest(rel[1]).find(rel[0]).slideUp(); }
			else{ $(rel[0]).slideUp(); }
			el.find('.show, .show-advanced').show();
			el.find('.hide, .hide-advanced').hide();
			el.removeClass('show-search');
		}else{
			if( rel.length > 1 ){ el.closest(rel[1]).find(rel[0]).slideDown(); }
			else{ $(rel[0]).slideDown(); }
			el.find('.show, .show-advanced').hide();
			el.find('.hide, .hide-advanced').show();
			el.addClass('show-search');
		}
		
	});
	if( EM.search_term_placeholder ){
		if( 'placeholder' in document.createElement('input') ){
			$('input.em-events-search-text, input.em-search-text').attr('placeholder', EM.search_term_placeholder);
		}else{
			$('input.em-events-search-text, input.em-search-text').blur(function(){
				if( this.value=='' ) this.value = EM.search_term_placeholder;
			}).focus(function(){
				if( this.value == EM.search_term_placeholder ) this.value='';
			}).trigger('blur');
		}
	}
	$('.em-search-form select[name=country]').change( function(){
		var el = $(this);
		$('.em-search select[name=state]').html('<option value="">'+EM.txt_loading+'</option>');
		$('.em-search select[name=region]').html('<option value="">'+EM.txt_loading+'</option>');
		$('.em-search select[name=town]').html('<option value="">'+EM.txt_loading+'</option>');
		if( el.val() != '' ){
			el.closest('.em-search-location').find('.em-search-location-meta').slideDown();
			var data = {
				action : 'search_states',
				country : el.val(),
				return_html : true
			};
			$('.em-search select[name=state]').load( EM.ajaxurl, data );
			data.action = 'search_regions';
			$('.em-search select[name=region]').load( EM.ajaxurl, data );
			data.action = 'search_towns';
			$('.em-search select[name=town]').load( EM.ajaxurl, data );
		}else{
			el.closest('.em-search-location').find('.em-search-location-meta').slideUp();
		}
	});

	$('.em-search-form select[name=region]').change( function(){
		$('.em-search select[name=state]').html('<option value="">'+EM.txt_loading+'</option>');
		$('.em-search select[name=town]').html('<option value="">'+EM.txt_loading+'</option>');
		var data = {
			action : 'search_states',
			region : $(this).val(),
			country : $('.em-search-form select[name=country]').val(),
			return_html : true
		};
		$('.em-search select[name=state]').load( EM.ajaxurl, data );
		data.action = 'search_towns';
		$('.em-search select[name=town]').load( EM.ajaxurl, data );
	});

	$('.em-search-form select[name=state]').change( function(){
		$('.em-search select[name=town]').html('<option value="">'+EM.txt_loading+'</option>');
		var data = {
			action : 'search_towns',
			state : $(this).val(),
			region : $('.em-search-form select[name=region]').val(),
			country : $('.em-search-form select[name=country]').val(),
			return_html : true
		};
		$('.em-search select[name=town]').load( EM.ajaxurl, data );
	});
	
	//in order for this to work, you need the above classes to be present in your templates
	$(document).delegate('.em-search-form, .em-events-search-form', 'submit', function(e){
		var form = $(this);
    	if( this.em_search && this.em_search.value == EM.txt_search){ this.em_search.value = ''; }
    	var results_wrapper = form.closest('.em-search-wrapper').find('.em-search-ajax');
    	if( results_wrapper.length == 0 ) results_wrapper = $('.em-search-ajax');
    	if( results_wrapper.length > 0 ){
    		results_wrapper.append('<div class="loading" id="em-loading"></div>');
    		var submitButton = form.find('.em-search-submit');
    		submitButton.data('buttonText', submitButton.val()).val(EM.txt_searching);
    		var img = submitButton.children('img');
    		if( img.length > 0 ) img.attr('src', img.attr('src').replace('search-mag.png', 'search-loading.gif'));
    		var vars = form.serialize();
    		$.ajax( EM.ajaxurl, {
				type : 'POST',
	    		dataType : 'html',
	    		data : vars,
			    success : function(responseText){
			    	submitButton.val(submitButton.data('buttonText'));
			    	if( img.length > 0 ) img.attr('src', img.attr('src').replace('search-loading.gif', 'search-mag.png'));
		    		results_wrapper.replaceWith(responseText);
		        	if( form.find('input[name=em_search]').val() == '' ){ form.find('input[name=em_search]').val(EM.txt_search); }
		        	//reload results_wrapper
		        	results_wrapper = form.closest('.em-search-wrapper').find('.em-search-ajax');
		        	if( results_wrapper.length == 0 ) results_wrapper = $('.em-search-ajax');
			    	jQuery(document).triggerHandler('em_search_ajax', [vars, results_wrapper, e]); //ajax has loaded new results
			    }
	    	});
    		e.preventDefault();
			return false;
    	}
	});
	if( $('.em-search-ajax').length > 0 ){
		$(document).delegate('.em-search-ajax a.page-numbers', 'click', function(e){
			var a = $(this);
			var data = a.closest('.em-pagination').attr('data-em-ajax');
			var wrapper = a.closest('.em-search-ajax');
			var wrapper_parent = wrapper.parent();
		    var qvars = a.attr('href').split('?');
		    var vars = qvars[1];
		    //add data-em-ajax att if it exists
		    if( data != '' ){
		    	vars = vars != '' ? vars+'&'+data : data;
		    }
		    wrapper.append('<div class="loading" id="em-loading"></div>');
		    $.ajax( EM.ajaxurl, {
				type : 'POST',
	    		dataType : 'html',
	    		data : vars,
			    success : function(responseText) {
			    	wrapper.replaceWith(responseText);
			    	wrapper = wrapper_parent.find('.em-search-ajax');
			    	jQuery(document).triggerHandler('em_search_ajax', [vars, wrapper, e]); //ajax has loaded new results
			    }
	    	});
			e.preventDefault();
			return false;
		});
	}
		
	/*
	 * ADMIN AREA AND PUBLIC FORMS (Still polishing this section up, note that form ids and classes may change accordingly)
	 */
	//Events List
		//Approve/Reject Links
		$('.events-table').on('click', '.em-event-delete', function(){
			if( !confirm("Are you sure you want to delete?") ){ return false; }
			window.location.href = this.href;
		});
	//Forms
	$('#event-form #event-image-delete, #location-form #location-image-delete').on('click', function(){
		var el = $(this);
		if( el.is(':checked') ){
			el.closest('.event-form-image, .location-form-image').find('#event-image-img, #location-image-img').hide();
		}else{
			el.closest('.event-form-image, .location-form-image').find('#event-image-img, #location-image-img').show();
		}
	});
	//Event Editor 
		//Recurrence Warnings
		$('#event-form.em-event-admin-recurring').submit( function(event){
			var form = $(this);
			if( form.find('input[name="event_reschedule"]').first().val() == 1 ){
				var warning_text = EM.event_reschedule_warning;
			}else if( form.find('input[name="event_recreate_tickets"]').first().val() == 1 ){
				var warning_text = EM.event_recurrence_bookings;
			}else{
				var warning_text = EM.event_recurrence_overwrite;
			}
			confirmation = confirm(warning_text);
			if( confirmation == false ){
				event.preventDefault();
			}
		});
		//Buttons for recurrence warnings within event editor forms
		$('.em-reschedule-trigger').click(function(e){
			e.preventDefault();
			var trigger = $(this);
			trigger.closest('.em-recurrence-reschedule').find(trigger.data('target')).removeClass('reschedule-hidden');
			trigger.siblings('.em-reschedule-value').val(1);
			trigger.addClass('reschedule-hidden').siblings('a').removeClass('reschedule-hidden');
		});
		$('.em-reschedule-cancel').click(function(e){
			e.preventDefault();
			var trigger = $(this);
			trigger.closest('.em-recurrence-reschedule').find(trigger.data('target')).addClass('reschedule-hidden');
			trigger.siblings('.em-reschedule-value').val(0);
			trigger.addClass('reschedule-hidden').siblings('a').removeClass('reschedule-hidden');
		});
	//Tickets & Bookings
	if( $("#em-tickets-form").length > 0 ){
		//Enable/Disable Bookings
		$('#event-rsvp').click( function(event){
			if( !this.checked ){
				confirmation = confirm(EM.disable_bookings_warning);
				if( confirmation == false ){
					event.preventDefault();
				}else{
					$('#event-rsvp-options').hide();
				}
			}else{
				$('#event-rsvp-options').fadeIn();
			}
		});
		if($('input#event-rsvp').is(":checked")) {
			$("div#rsvp-data").fadeIn();
		} else {
			$("div#rsvp-data").hide();
		}
		//Ticket(s) UI
		var reset_ticket_forms = function(){
			$('#em-tickets-form table tbody tr.em-tickets-row').show();
			$('#em-tickets-form table tbody tr.em-tickets-row-form').hide();
		};
		//recurrences and cut-off logic for ticket availability
		if( $('#em-recurrence-checkbox').length > 0 ){
			$('#em-recurrence-checkbox').change(function(){
				if( $('#em-recurrence-checkbox').is(':checked') ){
					$('#em-tickets-form .ticket-dates-from-recurring, #em-tickets-form .ticket-dates-to-recurring, #event-rsvp-options .em-booking-date-recurring').show();
					$('#em-tickets-form .ticket-dates-from-normal, #em-tickets-form .ticket-dates-to-normal, #event-rsvp-options .em-booking-date-normal, #em-tickets-form .hidden').hide();
				}else{
					$('#em-tickets-form .ticket-dates-from-normal, #em-tickets-form .ticket-dates-to-normal, #event-rsvp-options .em-booking-date-normal').show();
					$('#em-tickets-form .ticket-dates-from-recurring, #em-tickets-form .ticket-dates-to-recurring, #event-rsvp-options .em-booking-date-recurring, #em-tickets-form .hidden').hide();
				}
			}).trigger('change');
		}else if( $('#em-form-recurrence').length > 0 ){
			$('#em-tickets-form .ticket-dates-from-recurring, #em-tickets-form .ticket-dates-to-recurring, #event-rsvp-options .em-booking-date-recurring').show();
			$('#em-tickets-form .ticket-dates-from-normal, #em-tickets-form .ticket-dates-to-normal, #event-rsvp-options .em-booking-date-normal, #em-tickets-form .hidden').hide();
		}else{
			$('#em-tickets-form .ticket-dates-from-recurring, #em-tickets-form .ticket-dates-to-recurring, #event-rsvp-options .em-booking-date-recurring, #em-tickets-form .hidden').hide();
		}
		//Add a new ticket
		$("#em-tickets-add").click(function(e){ 
			e.preventDefault();
			reset_ticket_forms();
			//create copy of template slot, insert so ready for population
			var tickets = $('#em-tickets-form table tbody');
			var rowNo = tickets.length+1;
			var slot = tickets.first().clone(true).attr('id','em-ticket-'+ rowNo).appendTo($('#em-tickets-form table'));
			//change the index of the form element names
			slot.find('*[name]').each( function(index,el){
				el = $(el);
				el.attr('name', el.attr('name').replace('em_tickets[0]','em_tickets['+rowNo+']'));
			});
			//show ticket and switch to editor
			slot.show().find('.ticket-actions-edit').trigger('click');
			//refresh datepicker and values
			slot.find('.em-date-input-loc').datepicker('destroy').removeAttr('id'); //clear all datepickers
			slot.find('.em-time-input').unbind().each(function(index, el){ this.timePicker = false; }); //clear all timepickers - consequently, also other click/blur/change events, recreate the further down
			em_setup_datepicker(slot);
			em_setup_timepicker(slot);
		    $('html, body').animate({ scrollTop: slot.offset().top - 30 }); //sends user to form
		});
		//Edit a Ticket
		$(document).delegate('.ticket-actions-edit', 'click', function(e){
			e.preventDefault();
			reset_ticket_forms();
			var tbody = $(this).closest('tbody');
			tbody.find('tr.em-tickets-row').hide();
			tbody.find('tr.em-tickets-row-form').fadeIn();
			return false;
		});
		$(document).delegate('.ticket-actions-edited', 'click', function(e){
			e.preventDefault();
			var tbody = $(this).closest('tbody');
			var rowNo = tbody.attr('id').replace('em-ticket-','');
			tbody.find('.em-tickets-row').fadeIn();
			tbody.find('.em-tickets-row-form').hide();
			tbody.find('*[name]').each(function(index,el){
				el = $(el);
				if( el.attr('name') == 'ticket_start_pub'){
					tbody.find('span.ticket_start').text(el.attr('value'));
				}else if( el.attr('name') == 'ticket_end_pub' ){
					tbody.find('span.ticket_end').text(el.attr('value'));
				}else if( el.attr('name') == 'em_tickets['+rowNo+'][ticket_type]' ){
					if( el.find(':selected').val() == 'members' ){
						tbody.find('span.ticket_name').prepend('* ');
					}
				}else if( el.attr('name') == 'em_tickets['+rowNo+'][ticket_start_recurring_days]' ){
					var text = tbody.find('select.ticket-dates-from-recurring-when').val() == 'before' ? '-'+el.attr('value'):el.attr('value');
					if( el.attr('value') != '' ){
						tbody.find('span.ticket_start_recurring_days').text(text);
						tbody.find('span.ticket_start_recurring_days_text, span.ticket_start_time').removeClass('hidden').show();
					}else{
						tbody.find('span.ticket_start_recurring_days').text(' - ');
						tbody.find('span.ticket_start_recurring_days_text, span.ticket_start_time').removeClass('hidden').hide();
					}
				}else if( el.attr('name') == 'em_tickets['+rowNo+'][ticket_end_recurring_days]' ){
					var text = tbody.find('select.ticket-dates-to-recurring-when').val() == 'before' ? '-'+el.attr('value'):el.attr('value');
					if( el.attr('value') != '' ){
						tbody.find('span.ticket_end_recurring_days').text(text);
						tbody.find('span.ticket_end_recurring_days_text, span.ticket_end_time').removeClass('hidden').show();
					}else{
						tbody.find('span.ticket_end_recurring_days').text(' - ');
						tbody.find('span.ticket_end_recurring_days_text, span.ticket_end_time').removeClass('hidden').hide();
					}
				}else{
					tbody.find('.'+el.attr('name').replace('em_tickets['+rowNo+'][','').replace(']','').replace('[]','')).text(el.attr('value'));
				}
			});
			//allow for others to hook into this
			$(document).triggerHandler('em_maps_tickets_edit', [tbody, rowNo, true]);
		    $('html, body').animate({ scrollTop: tbody.parent().offset().top - 30 }); //sends user back to top of form
			return false;
		});
		$(document).delegate('.em-ticket-form select.ticket_type','change', function(e){
			//check if ticket is for all users or members, if members, show roles to limit the ticket to
			var el = $(this);
			if( el.find('option:selected').val() == 'members' ){
				el.closest('.em-ticket-form').find('.ticket-roles').fadeIn();
			}else{
				el.closest('.em-ticket-form').find('.ticket-roles').hide();
			}
		});
		$(document).delegate('.em-ticket-form .ticket-options-advanced','click', function(e){
			//show or hide advanced tickets, hidden by default
			e.preventDefault();
			var el = $(this);
			if( el.hasClass('show') ){
				el.closest('.em-ticket-form').find('.em-ticket-form-advanced').fadeIn();
				el.find('.show,.show-advanced').hide();
				el.find('.hide,.hide-advanced').show();
			}else{
				el.closest('.em-ticket-form').find('.em-ticket-form-advanced').hide();
				el.find('.show,.show-advanced').show();
				el.find('.hide,.hide-advanced').hide();
			}
			el.toggleClass('show');
		});
		$('.em-ticket-form').each( function(){
			//check whether to show advanced options or not by default for each ticket
			var show_advanced = false;
			var el = $(this); 
			el.find('.em-ticket-form-advanced input[type="text"]').each(function(){ if(this.value != '') show_advanced = true; });
			if( el.find('.em-ticket-form-advanced input[type="checkbox"]:checked').length > 0 ){ show_advanced = true; }
			el.find('.em-ticket-form-advanced option:selected').each(function(){ if(this.value != '') show_advanced = true; });
			if( show_advanced ) el.find('.ticket-options-advanced').trigger('click');
		});
		//Delete a ticket
		$(document).delegate('.ticket-actions-delete', 'click', function(e){
			e.preventDefault();
			var el = $(this);
			var tbody = el.closest('tbody');
			if( tbody.find('input.ticket_id').val() > 0 ){
				//only will happen if no bookings made
				el.text('Deleting...');	
				$.getJSON( $(this).attr('href'), {'em_ajax_action':'delete_ticket', 'id':tbody.find('input.ticket_id').val()}, function(data){
					if(data.result){
						tbody.remove();
					}else{
						el.text('Delete');
						alert(data.error);
					}
				});
			}else{
				//not saved to db yet, so just remove
				tbody.remove();
			}
			return false;
		});
	}
	//Manageing Bookings
	if( $('#em-bookings-table').length > 0 ){
		//Pagination link clicks
		$(document).delegate('#em-bookings-table .tablenav-pages a', 'click', function(){
			var el = $(this);
			var form = el.parents('#em-bookings-table form.bookings-filter');
			//get page no from url, change page, submit form
			var match = el.attr('href').match(/#[0-9]+/);
			if( match != null && match.length > 0){
				var pno = match[0].replace('#','');
				form.find('input[name=pno]').val(pno);
			}else{
				form.find('input[name=pno]').val(1);
			}
			form.trigger('submit');
			return false;
		});
		//Overlay Options
		var em_bookings_settings_dialog = {
			modal : true,
			autoOpen: false,
			minWidth: 500,
			height: 'auto',
			buttons: [{
				text: EM.bookings_settings_save,
				click: function(e){
					e.preventDefault();
					//we know we'll deal with cols, so wipe hidden value from main
					var match = $("#em-bookings-table form.bookings-filter [name=cols]").val('');
					var booking_form_cols = $('form#em-bookings-table-settings-form input.em-bookings-col-item');
					$.each( booking_form_cols, function(i,item_match){
						//item_match = $(item_match);
						if( item_match.value == 1 ){
							if( match.val() != ''){
								match.val(match.val()+','+item_match.name);
							}else{
								match.val(item_match.name);
							}
						}
					});
					//submit main form
					$('#em-bookings-table-settings').trigger('submitted'); //hook into this with bind()
					$('#em-bookings-table form.bookings-filter').trigger('submit');					
					$(this).dialog('close');
				}
			}]
		};
		var em_bookings_export_dialog = {
			modal : true,
			autoOpen: false,
			minWidth: 500,
			height: 'auto',
			buttons: [{
				text: EM.bookings_export_save,
				click: function(e){
					$(this).children('form').submit();
					$(this).dialog('close');
				}
			}]
		};
		if( $("#em-bookings-table-settings").length > 0 ){
			//Settings Overlay
			$("#em-bookings-table-settings").dialog(em_bookings_settings_dialog);
			$(document).delegate('#em-bookings-table-settings-trigger','click', function(e){ e.preventDefault(); $("#em-bookings-table-settings").dialog('open'); });
			//Export Overlay
			$("#em-bookings-table-export").dialog(em_bookings_export_dialog);
			$(document).delegate('#em-bookings-table-export-trigger','click', function(e){ e.preventDefault(); $("#em-bookings-table-export").dialog('open'); });
			var export_overlay_show_tickets = function(){
				if( $('#em-bookings-table-export-form input[name=show_tickets]').is(':checked') ){
					$('#em-bookings-table-export-form .em-bookings-col-item-ticket').show();
					$('#em-bookings-table-export-form #em-bookings-export-cols-active .em-bookings-col-item-ticket input').val(1);
				}else{
					$('#em-bookings-table-export-form .em-bookings-col-item-ticket').hide().find('input').val(0);					
				}
			};
			//Sync export overlay with table search field changes
			$('#em-bookings-table form select').each(function(i, el){
				$(el).change(function(e){
					var select_el = $(this);
					var input_par = $('#em-bookings-table-export-form input[name='+select_el.attr('name')+']');
					var input_par_selected = select_el.find('option:selected');
					input_par.val(input_par_selected.val());
				});
			});
			
			export_overlay_show_tickets();
			$('#em-bookings-table-export-form input[name=show_tickets]').click(export_overlay_show_tickets);
			//Sortables
			$( ".em-bookings-cols-sortable" ).sortable({
				connectWith: ".em-bookings-cols-sortable",
				update: function(event, ui) {
					if( ui.item.parents('ul#em-bookings-cols-active, ul#em-bookings-export-cols-active').length > 0 ){							
						ui.item.addClass('ui-state-highlight').removeClass('ui-state-default').children('input').val(1);
					}else{
						ui.item.addClass('ui-state-default').removeClass('ui-state-highlight').children('input').val(0);
					}
				}
			}).disableSelection();
			load_ui_css = true;
		}
		//Widgets and filter submissions
		$(document).delegate('#em-bookings-table form.bookings-filter', 'submit', function(e){
			var el = $(this);			
			//append loading spinner
			el.parents('#em-bookings-table').find('.table-wrap').first().append('<div id="em-loading" />');
			//ajax call
			$.post( EM.ajaxurl, el.serializeArray(), function(data){
				var root = el.parents('#em-bookings-table').first();
				root.replaceWith(data);
				//recreate overlays
				$('#em-bookings-table-export input[name=scope]').val(root.find('select[name=scope]').val());
				$('#em-bookings-table-export input[name=status]').val(root.find('select[name=status]').val());
				jQuery(document).triggerHandler('em_bookings_filtered', [data, root, el]);
			});
			return false;
		});
		//Approve/Reject Links
		$(document).delegate('.em-bookings-approve,.em-bookings-reject,.em-bookings-unapprove,.em-bookings-delete', 'click', function(){
			var el = $(this); 
			if( el.hasClass('em-bookings-delete') ){
				if( !confirm(EM.booking_delete) ){ return false; }
			}
			var url = em_ajaxify( el.attr('href'));		
			var td = el.parents('td').first();
			td.html(EM.txt_loading);
			td.load( url );
			return false;
		});
	}
	//Old Bookings Table - depreciating soon
	if( $('.em_bookings_events_table').length > 0 ){
		//Widgets and filter submissions
		$(document).delegate('.em_bookings_events_table form', 'submit', function(e){
			var el = $(this);
			var url = em_ajaxify( el.attr('action') );		
			el.parents('.em_bookings_events_table').find('.table-wrap').first().append('<div id="em-loading" />');
			$.get( url, el.serializeArray(), function(data){
				el.parents('.em_bookings_events_table').first().replaceWith(data);
			});
			return false;
		});
		//Pagination link clicks
		$(document).delegate('.em_bookings_events_table .tablenav-pages a', 'click', function(){		
			var el = $(this);
			var url = em_ajaxify( el.attr('href') );	
			el.parents('.em_bookings_events_table').find('.table-wrap').first().append('<div id="em-loading" />');
			$.get( url, function(data){
				el.parents('.em_bookings_events_table').first().replaceWith(data);
			});
			return false;
		});
	}
	
	//Manual Booking
	$('a.em-booking-button').click(function(e){
		e.preventDefault();
		var button = $(this);
		if( button.text() != EM.bb_booked && $(this).text() != EM.bb_booking){
			button.text(EM.bb_booking);
			var button_data = button.attr('id').split('_'); 
			$.ajax({
				url: EM.ajaxurl,
				dataType: 'jsonp',
				data: {
					event_id : button_data[1],
					_wpnonce : button_data[2],
					action : 'booking_add_one'
				},
				success : function(response, statusText, xhr, $form) {
					if(response.result){
						button.text(EM.bb_booked);
					}else{
						button.text(EM.bb_error);					
					}
					if(response.message != '') alert(response.message);
				},
				error : function(){ button.text(EM.bb_error); }
			});
		}
		return false;
	});	
	$('a.em-cancel-button').click(function(e){
		e.preventDefault();
		var button = $(this);
		if( button.text() != EM.bb_cancelled && button.text() != EM.bb_canceling){
			button.text(EM.bb_canceling);
			var button_data = button.attr('id').split('_'); 
			$.ajax({
				url: EM.ajaxurl,
				dataType: 'jsonp',
				data: {
					booking_id : button_data[1],
					_wpnonce : button_data[2],
					action : 'booking_cancel'
				},
				success : function(response, statusText, xhr, $form) {
					if(response.result){
						button.text(EM.bb_cancelled);
					}else{
						button.text(EM.bb_cancel_error);
					}
				},
				error : function(){ button.text(EM.bb_cancel_error); }
			});
		}
		return false;
	});  

	//Datepicker
	if( $('.em-date-single, .em-date-range, #em-date-start').length > 0 ){
		load_ui_css = true;
		em_setup_datepicker('body');
	}
	if( load_ui_css ) em_load_jquery_css();
	
	//previously in em-admin.php
	function updateIntervalDescriptor () { 
		$(".interval-desc").hide();
		var number = "-plural";
		if ($('input#recurrence-interval').val() == 1 || $('input#recurrence-interval').val() == "")
		number = "-singular";
		var descriptor = "span#interval-"+$("select#recurrence-frequency").val()+number;
		$(descriptor).show();
	}
	function updateIntervalSelectors () {
		$('p.alternate-selector').hide();   
		$('p#'+ $('select#recurrence-frequency').val() + "-selector").show();
	}
	function updateShowHideRecurrence () {
		if( $('input#event-recurrence').is(":checked")) {
			$("#event_recurrence_pattern").fadeIn();
			$("#event-date-explanation").hide();
			$("#recurrence-dates-explanation").show();
			$("h3#recurrence-dates-title").show();
			$("h3#event-date-title").hide();     
		} else {
			$("#event_recurrence_pattern").hide();
			$("#recurrence-dates-explanation").hide();
			$("#event-date-explanation").show();
			$("h3#recurrence-dates-title").hide();
			$("h3#event-date-title").show();   
		}
	}		 
	$("#recurrence-dates-explanation").hide();
	$("#date-to-submit").hide();
	$("#end-date-to-submit").hide();
	
	$("#localised-date").show();
	$("#localised-end-date").show();
	
	$('#em-wrapper input.select-all').change(function(){
	 	if($(this).is(':checked')){
			$('input.row-selector').prop('checked', true);
			$('input.select-all').prop('checked', true);
	 	}else{
			$('input.row-selector').prop('checked', false);
			$('input.select-all').prop('checked', false);
		}
	}); 
	
	updateIntervalDescriptor(); 
	updateIntervalSelectors();
	updateShowHideRecurrence();
	$('input#event-recurrence').change(updateShowHideRecurrence);
	   
	// recurrency elements   
	$('input#recurrence-interval').keyup(updateIntervalDescriptor);
	$('select#recurrence-frequency').change(updateIntervalDescriptor);
	$('select#recurrence-frequency').change(updateIntervalSelectors);

	/* Load any maps */	
	if( $('.em-location-map').length > 0 || $('.em-locations-map').length > 0 || $('#em-map').length > 0 || $('.em-search-geo').length > 0 ){
		em_maps_load();
	}
	
	//Finally, add autocomplete here
	//Autocomplete
	if( jQuery( "div.em-location-data input#location-name" ).length > 0 ){
		jQuery( "div.em-location-data input#location-name" ).autocomplete({
			source: EM.locationajaxurl,
			minLength: 2,
			focus: function( event, ui ){
				jQuery("input#location-id" ).val( ui.item.value );
				return false;
			},			 
			select: function( event, ui ){
				jQuery("input#location-id" ).val(ui.item.id).trigger('change');
				jQuery("input#location-name" ).val(ui.item.value);
				jQuery('input#location-address').val(ui.item.address);
				jQuery('input#location-town').val(ui.item.town);
				jQuery('input#location-state').val(ui.item.state);
				jQuery('input#location-region').val(ui.item.region);
				jQuery('input#location-postcode').val(ui.item.postcode);
				if( ui.item.country == '' ){
					jQuery('select#location-country option:selected').removeAttr('selected');
				}else{
					jQuery('select#location-country option[value="'+ui.item.country+'"]').attr('selected', 'selected');
				}
				jQuery('div.em-location-data input, div.em-location-data select').css('background-color','#ccc').attr('readonly','readonly');
				jQuery('#em-location-reset').show();
				jQuery('#em-location-search-tip').hide();
				jQuery(document).triggerHandler('em_locations_autocomplete_selected', [event, ui]);
				return false;
			}
		}).data( "ui-autocomplete" )._renderItem = function( ul, item ) {
			html_val = "<a>" + item.label + '<br><span style="font-size:11px"><em>'+ item.address + ', ' + item.town+"</em></span></a>";
			return jQuery( "<li></li>" ).data( "item.autocomplete", item ).append(html_val).appendTo( ul );
		};
		jQuery('#em-location-reset a').click( function(){
			jQuery('div.em-location-data input').css('background-color','#fff').val('').removeAttr('readonly');
			jQuery('div.em-location-data select').css('background-color','#fff');
			jQuery('div.em-location-data option:selected').removeAttr('selected');
			jQuery('input#location-id').val('');
			jQuery('#em-location-reset').hide();
			jQuery('#em-location-search-tip').show();
			jQuery('#em-map').hide();
			jQuery('#em-map-404').show();
			if(typeof(marker) !== 'undefined'){
				marker.setPosition(new google.maps.LatLng(0, 0));
				infoWindow.close();
				marker.setDraggable(true);
			}
			return false;
		});
		if( jQuery('input#location-id').val() != '0' && jQuery('input#location-id').val() != '' ){
			jQuery('div.em-location-data input, div.em-location-data select').css('background-color','#ccc').attr('readonly','readonly');
			jQuery('#em-location-reset').show();
			jQuery('#em-location-search-tip').hide();
		}
	}
	
});

function em_load_jquery_css(){
	if( EM.ui_css && jQuery('link#jquery-ui-css').length == 0 ){
		var script = document.createElement("link");
		script.id = 'jquery-ui-css';
		script.rel = "stylesheet";
		script.href = EM.ui_css;
		document.body.appendChild(script);
	}
}

function em_setup_datepicker(wrap){	
	wrap = jQuery(wrap);
	//default picker vals
	var datepicker_vals = { altFormat: "yy-mm-dd", changeMonth: true, changeYear: true, firstDay : EM.firstDay, yearRange:'-100:+10' };
	if( EM.dateFormat ) datepicker_vals.dateFormat = EM.dateFormat;
	if( EM.yearRange ) datepicker_vals.yearRange = EM.yearRange;
	jQuery(document).triggerHandler('em_datepicker', datepicker_vals);
	
	//apply datepickers
	dateDivs = wrap.find('.em-date-single, .em-date-range');
	if( dateDivs.length > 0 ){
		//apply datepickers to elements
		dateDivs.find('input.em-date-input-loc').each(function(i,dateInput){
			//init the datepicker
			var dateInput = jQuery(dateInput);
			var dateValue = dateInput.nextAll('input.em-date-input').first();
			var dateValue_value = dateValue.val();
			dateInput.datepicker(datepicker_vals);
			dateInput.datepicker('option', 'altField', dateValue);
			//now set the value
			if( dateValue_value ){
				var this_date_formatted = jQuery.datepicker.formatDate( EM.dateFormat, jQuery.datepicker.parseDate('yy-mm-dd', dateValue_value) );
				dateInput.val(this_date_formatted);
				dateValue.val(dateValue_value);
			}
			//add logic for texts
			dateInput.change(function(){
				if( jQuery(this).val() == '' ){
					jQuery(this).nextAll('.em-date-input').first().val('');
				}
			});
		});
		//deal with date ranges
		dateDivs.filter('.em-date-range').find('input.em-date-input-loc').each(function(i,dateInput){
			//finally, apply start/end logic to this field
			dateInput = jQuery(dateInput);
			if( dateInput.hasClass('em-date-start') ){
				dateInput.datepicker('option','onSelect', function( selectedDate ) {
					//get corresponding end date input, we expect ranges to be contained in .em-date-range with a start/end input element
					var startDate = jQuery(this);
					var endDate = startDate.parents('.em-date-range').find('.em-date-end').first();
					var startValue = startDate.nextAll('input.em-date-input').first().val();
					var endValue = endDate.nextAll('input.em-date-input').first().val();
					if( startValue > endValue && endValue != '' ){
						endDate.datepicker( "setDate" , selectedDate );
						endDate.trigger('change');
					}
					endDate.datepicker( "option", 'minDate', selectedDate );
				});
			}else if( dateInput.hasClass('em-date-end') ){
				var startInput = dateInput.parents('.em-date-range').find('.em-date-start').first();
				if( startInput.val() != '' ){
					dateInput.datepicker('option', 'minDate', startInput.val());
				}
			}
		});
	}
}

function em_setup_timepicker(wrap){
	wrap = jQuery(wrap);
	var timepicker_options = {
		show24Hours: EM.show24hours == 1,
		step:15
	}
	jQuery(document).triggerHandler('em_timepicker_options', timepicker_options);
	wrap.find(".em-time-input").timePicker(timepicker_options);
	
	// Keep the duration between the two inputs.
	wrap.find(".em-time-range input.em-time-start").each( function(i, el){
		jQuery(el).data('oldTime', jQuery.timePicker(el).getTime());
	}).change( function() {
		var start = jQuery(this);
		var end = start.nextAll('.em-time-end');
		if (end.val()) { // Only update when second input has a value.
		    // Calculate duration.
			var oldTime = start.data('oldTime');
		    var duration = (jQuery.timePicker(end).getTime() - oldTime);
		    var time = jQuery.timePicker(start).getTime();
		    if( jQuery.timePicker(end).getTime() >= oldTime ){
			    // Calculate and update the time in the second input.
			    jQuery.timePicker(end).setTime(new Date(new Date(time.getTime() + duration)));
			}
		    start.data('oldTime', time); 
		}
	});
	// Validate.
	wrap.find(".em-time-range input.em-time-end").change(function() {
		var end = jQuery(this);
		var start = end.prevAll('.em-time-start');
		if( start.val() ){
			if( jQuery.timePicker(start).getTime() > jQuery.timePicker(this).getTime() && ( jQuery('.em-date-end').val().length == 0 || jQuery('.em-date-start').val() == jQuery('.em-date-end').val() ) ) { end.addClass("error"); }
			else { end.removeClass("error"); }
		}
	});
	//Sort out all day checkbox
	wrap.find('.em-time-range input.em-time-all-day').change(function(){
		var allday = jQuery(this);
		if( allday.is(':checked') ){
			allday.siblings('.em-time-input').css('background-color','#ccc');
		}else{
			allday.siblings('.em-time-input').css('background-color','#fff');
		}
	}).trigger('change');
}

/* Useful function for adding the em_ajax flag to a url, regardless of querystring format */
var em_ajaxify = function(url){
	if ( url.search('em_ajax=0') != -1){
		url = url.replace('em_ajax=0','em_ajax=1');
	}else if( url.search(/\?/) != -1 ){
		url = url + "&em_ajax=1";
	}else{
		url = url + "?em_ajax=1";
	}
	return url;
};

/*
 * MAP FUNCTIONS
 */
var em_maps_loaded = false;
var maps = {};
var maps_markers = {};
var infowindow;
//loads maps script if not already loaded and executes EM maps script
function em_maps_load(){
	if( !em_maps_loaded ){
		if ( jQuery('script#google-maps').length == 0 && ( typeof google !== 'object' || typeof google.maps !== 'object' ) ){ 
			var script = document.createElement("script");
			script.type = "text/javascript";
			script.id = "google-maps";
			var proto = (EM.is_ssl) ? 'https:' : 'http:';
			if( typeof EM.google_maps_api !== 'undefined' ){
				script.src = proto + '//maps.google.com/maps/api/js?v=3&libraries=places&callback=em_maps&key='+EM.google_maps_api;
			}else{
				script.src = proto + '//maps.google.com/maps/api/js?v=3&libraries=places&callback=em_maps';
			}
			document.body.appendChild(script);
		}else if( typeof google === 'object' && typeof google.maps === 'object' && !em_maps_loaded ){
			em_maps();
		}else if( jQuery('script#google-maps').length > 0 ){
			jQuery(window).load(function(){ if( !em_maps_loaded ) em_maps(); }); //google isn't loaded so wait for page to load resources
		}
	}
}
//re-usable function to load global location maps
function em_maps_load_locations(el){
	var el = jQuery(el);
	var map_id = el.attr('id').replace('em-locations-map-','');
	var em_data = jQuery.parseJSON( el.nextAll('.em-locations-map-coords').first().text() );
	if( em_data == null ){
		var em_data = jQuery.parseJSON( jQuery('#em-locations-map-coords-'+map_id).text() );
	}
	jQuery.getJSON(document.URL, em_data , function(data){
		if(data.length > 0){
			//define default options and allow option for extension via event triggers
			  var map_options = { mapTypeId: google.maps.MapTypeId.ROADMAP };
			  if( typeof EM.google_map_id_styles == 'object' && typeof EM.google_map_id_styles[map_id] !== 'undefined' ){ console.log(EM.google_map_id_styles[map_id]); map_options.styles = EM.google_map_id_styles[map_id]; }
			  else if( typeof EM.google_maps_styles !== 'undefined' ){ map_options.styles = EM.google_maps_styles; }
			  jQuery(document).triggerHandler('em_maps_locations_map_options', map_options);
			  var marker_options = {};
			  jQuery(document).triggerHandler('em_maps_location_marker_options', marker_options);
			  
			  maps[map_id] = new google.maps.Map(el[0], map_options);
			  maps_markers[map_id] = [];

			  var bounds = new google.maps.LatLngBounds();
			  
			  jQuery.map( data, function( location, i ){
				  if( !(location.location_latitude == 0 && location.location_longitude == 0) ){
					var latitude = parseFloat( location.location_latitude );
					var longitude = parseFloat( location.location_longitude );
					var location_position = new google.maps.LatLng( latitude, longitude );
					//extend the default marker options
					jQuery.extend(marker_options, {
					    position: location_position, 
					    map: maps[map_id]
					})
					var marker = new google.maps.Marker(marker_options);
					maps_markers[map_id].push(marker);
					marker.setTitle(location.location_name);
					var myContent = '<div class="em-map-balloon"><div id="em-map-balloon-'+map_id+'" class="em-map-balloon-content">'+ location.location_balloon +'</div></div>';
					em_map_infobox(marker, myContent, maps[map_id]);
					//extend bounds
					bounds.extend(new google.maps.LatLng(latitude,longitude))
				  }
			  });
			  // Zoom in to the bounds
			  maps[map_id].fitBounds(bounds);
			  
			//Call a hook if exists
			jQuery(document).triggerHandler('em_maps_locations_hook', [maps[map_id], data, map_id, maps_markers[map_id]]);
		}else{
			el.children().first().html('No locations found');
			jQuery(document).triggerHandler('em_maps_locations_hook_not_found', [el]);
		}
	});
}
function em_maps_load_location(el){
	el = jQuery(el);
	var map_id = el.attr('id').replace('em-location-map-','');
	em_LatLng = new google.maps.LatLng( jQuery('#em-location-map-coords-'+map_id+' .lat').text(), jQuery('#em-location-map-coords-'+map_id+' .lng').text());
	//extend map and markers via event triggers
	var is_touch = 'ontouchstart' in window || navigator.maxTouchPoints;
	var map_options = {
	    zoom: 14,
	    center: em_LatLng,
	    mapTypeId: google.maps.MapTypeId.ROADMAP,
	    mapTypeControl: false,
	    scrollwheel: is_touch
	};
	if( typeof EM.google_map_id_styles == 'object' && typeof EM.google_map_id_styles[map_id] !== 'undefined' ){ console.log(EM.google_map_id_styles[map_id]); map_options.styles = EM.google_map_id_styles[map_id]; }
	else if( typeof EM.google_maps_styles !== 'undefined' ){ map_options.styles = EM.google_maps_styles; }
	jQuery(document).triggerHandler('em_maps_location_map_options', map_options);
	maps[map_id] = new google.maps.Map( document.getElementById('em-location-map-'+map_id), map_options);
	if( !is_touch ){
		maps[map_id].addListener('click', function(){ maps[map_id].setOptions({ scrollwheel:true }); });
		maps[map_id].addListener('mouseout', function(){ maps[map_id].setOptions({ scrollwheel:false }); });
	}
	var marker_options = {
	    position: em_LatLng,
	    map: maps[map_id]
	};
	jQuery(document).triggerHandler('em_maps_location_marker_options', marker_options);
	maps_markers[map_id] = new google.maps.Marker(marker_options);
	infowindow = new google.maps.InfoWindow({ content: jQuery('#em-location-map-info-'+map_id+' .em-map-balloon').get(0) });
	infowindow.open(maps[map_id],maps_markers[map_id]);
	maps[map_id].panBy(40,-70);
	
	//JS Hook for handling map after instantiation
	//Example hook, which you can add elsewhere in your theme's JS - jQuery(document).bind('em_maps_location_hook', function(){ alert('hi');} );
	jQuery(document).triggerHandler('em_maps_location_hook', [maps[map_id], infowindow, maps_markers[map_id], map_id]);
	//map resize listener
	jQuery(window).on('resize', function(e) {
		google.maps.event.trigger(maps[map_id], "resize");
		maps[map_id].setCenter(maps_markers[map_id].getPosition());
		maps[map_id].panBy(40,-70);
	});
}
jQuery(document).bind('em_search_ajax', function(e, vars, wrapper){
	if( em_maps_loaded ){
		wrapper.find('.em-location-map').each( function(index, el){ em_maps_load_location(el); } );
		wrapper.find('.em-locations-map').each( function(index, el){ em_maps_load_locations(el); });
	}
});
//Load single maps (each map is treated as a seperate map).
function em_maps() {
	//Find all the maps on this page and load them
	jQuery('.em-location-map').each( function(index, el){ em_maps_load_location(el); } );	
	jQuery('.em-locations-map').each( function(index, el){ em_maps_load_locations(el); } );
	
	//Location stuff - only needed if inputs for location exist
	if( jQuery('select#location-select-id, input#location-address').length > 0 ){
		var map, marker;
		//load map info
		var refresh_map_location = function(){
			var location_latitude = jQuery('#location-latitude').val();
			var location_longitude = jQuery('#location-longitude').val();
			if( !(location_latitude == 0 && location_longitude == 0) ){
				var position = new google.maps.LatLng(location_latitude, location_longitude); //the location coords
				marker.setPosition(position);
				var mapTitle = (jQuery('input#location-name').length > 0) ? jQuery('input#location-name').val():jQuery('input#title').val();
				marker.setTitle( jQuery('input#location-name input#title, #location-select-id').first().val() );
				jQuery('#em-map').show();
				jQuery('#em-map-404').hide();
				google.maps.event.trigger(map, 'resize');
				map.setCenter(position);
				map.panBy(40,-55);
				infoWindow.setContent( 
					'<div id="location-balloon-content"><strong>' + 
					mapTitle + 
					'</strong><br/>' + 
					jQuery('#location-address').val() + 
					'<br/>' + jQuery('#location-town').val()+ 
					'</div>'
				);
				infoWindow.open(map, marker);
				jQuery(document).triggerHandler('em_maps_location_hook', [map, infowindow, marker, 0]);
			} else {
    			jQuery('#em-map').hide();
    			jQuery('#em-map-404').show();
			}
		};
		
		//Add listeners for changes to address
		var get_map_by_id = function(id){
			if(jQuery('#em-map').length > 0){
				jQuery.getJSON(document.URL,{ em_ajax_action:'get_location', id:id }, function(data){
					if( data.location_latitude!=0 && data.location_longitude!=0 ){
						loc_latlng = new google.maps.LatLng(data.location_latitude, data.location_longitude);
						marker.setPosition(loc_latlng);
						marker.setTitle( data.location_name );
						marker.setDraggable(false);
						jQuery('#em-map').show();
						jQuery('#em-map-404').hide();
						map.setCenter(loc_latlng);
						map.panBy(40,-55);
						infoWindow.setContent( '<div id="location-balloon-content">'+ data.location_balloon +'</div>');
						infoWindow.open(map, marker);
						google.maps.event.trigger(map, 'resize');
						jQuery(document).triggerHandler('em_maps_location_hook', [map, infowindow, marker, 0]);
					}else{
						jQuery('#em-map').hide();
						jQuery('#em-map-404').show();
					}
				});
			}
		};
		jQuery('#location-select-id, input#location-id').change( function(){get_map_by_id(jQuery(this).val());} );
		jQuery('#location-name, #location-town, #location-address, #location-state, #location-postcode, #location-country').change( function(){
			//build address
			var addresses = [ jQuery('#location-address').val(), jQuery('#location-town').val(), jQuery('#location-state').val(), jQuery('#location-postcode').val() ];
			var address = '';
			jQuery.each( addresses, function(i, val){
				if( val != '' ){
					address = ( address == '' ) ? address+val:address+', '+val;
				}
			});
			if( address == '' ){ //in case only name is entered, no address
				jQuery('#em-map').hide();
				jQuery('#em-map-404').show();
				return false;
			}
			//do country last, as it's using the text version
			if( jQuery('#location-country option:selected').val() != 0 ){
				address = ( address == '' ) ? address+jQuery('#location-country option:selected').text():address+', '+jQuery('#location-country option:selected').text();
			}
			if( address != '' && jQuery('#em-map').length > 0 ){
				geocoder.geocode( { 'address': address }, function(results, status) {
				    if (status == google.maps.GeocoderStatus.OK) {
						jQuery('#location-latitude').val(results[0].geometry.location.lat());
						jQuery('#location-longitude').val(results[0].geometry.location.lng());
					}
				    refresh_map_location();
				});
			}
		});
		
		//Load map
		if(jQuery('#em-map').length > 0){
			var em_LatLng = new google.maps.LatLng(0, 0);
			var is_touch = 'ontouchstart' in window || navigator.maxTouchPoints;
			var map_options = {
				    zoom: 14,
				    center: em_LatLng,
				    mapTypeId: google.maps.MapTypeId.ROADMAP,
				    mapTypeControl: false,
				    scrollwheel: is_touch
			};
			if( typeof EM.google_maps_styles !== 'undefined' ){ map_options.styles = EM.google_maps_styles; }
			map = new google.maps.Map( document.getElementById('em-map'), map_options);
			if( !is_touch ){
				map.addListener('click', function(){ map.setOptions({ scrollwheel:true }); });
				map.addListener('mouseout', function(){ map.setOptions({ scrollwheel:false }); });
			}
			var marker = new google.maps.Marker({
			    position: em_LatLng,
			    map: map,
			    draggable: true
			});
			infoWindow = new google.maps.InfoWindow({
			    content: ''
			});
			var geocoder = new google.maps.Geocoder();
			google.maps.event.addListener(infoWindow, 'domready', function() { 
				document.getElementById('location-balloon-content').parentNode.style.overflow=''; 
				document.getElementById('location-balloon-content').parentNode.parentNode.style.overflow=''; 
			});
			google.maps.event.addListener(marker, 'dragend', function() {
				var position = marker.getPosition();
				jQuery('#location-latitude').val(position.lat());
				jQuery('#location-longitude').val(position.lng());
				map.setCenter(position);
				map.panBy(40,-55);
			});
			if( jQuery('#location-select-id').length > 0 ){
				jQuery('#location-select-id').trigger('change');
			}else{
				refresh_map_location();
			}
			jQuery(document).triggerHandler('em_map_loaded', [map, infowindow, marker]);
		}
		//map resize listener
		jQuery(window).on('resize', function(e) {
			google.maps.event.trigger(map, "resize");
			map.setCenter(marker.getPosition());
			map.panBy(40,-55);
		});
	}
	em_maps_loaded = true; //maps have been loaded
	jQuery(document).triggerHandler('em_maps_loaded');
}
  
function em_map_infobox(marker, message, map) {
  var iw = new google.maps.InfoWindow({ content: message });
  google.maps.event.addListener(marker, 'click', function() {
	if( infowindow ) infowindow.close();
	infowindow = iw;
    iw.open(map,marker);
  });
}

/* jQuery timePicker - http://labs.perifer.se/timedatepicker/ @ http://github.com/perifer/timePicker commit 100644 */
(function(e){function t(t,n,r,i){t.value=e(n).text();e(t).change();if(!navigator.userAgent.match(/msie/i)){t.focus()}r.hide()}function n(e,t){var n=e.getHours();var i=t.show24Hours?n:(n+11)%12+1;var s=e.getMinutes();return r(i)+t.separator+r(s)+(t.show24Hours?"":n<12?" AM":" PM")}function r(e){return(e<10?"0":"")+e}function i(e,t){return typeof e=="object"?o(e):s(e,t)}function s(e,t){if(e){var n=e.split(t.separator);var r=parseFloat(n[0]);var i=parseFloat(n[1]);if(!t.show24Hours){if(r===12&&e.indexOf("AM")!==-1){r=0}else if(r!==12&&e.indexOf("PM")!==-1){r+=12}}var s=new Date(0,0,0,r,i,0);return o(s)}return null}function o(e){e.setFullYear(2001);e.setMonth(0);e.setDate(0);return e}e.fn.timePicker=function(t){var n=e.extend({},e.fn.timePicker.defaults,t);return this.each(function(){e.timePicker(this,n)})};e.timePicker=function(t,n){var r=e(t)[0];return r.timePicker||(r.timePicker=new jQuery._timePicker(r,n))};e.timePicker.version="0.3";e._timePicker=function(r,u){var a=false;var f=false;var l=i(u.startTime,u);var c=i(u.endTime,u);var h="selected";var p="li."+h;e(r).attr("autocomplete","OFF");var d=[];var v=new Date(l);while(v<=c){d[d.length]=n(v,u);v=new Date(v.setMinutes(v.getMinutes()+u.step))}var m=e('<div class="time-picker'+(u.show24Hours?"":" time-picker-12hours")+'"></div>');var g=e("<ul></ul>");for(var y=0;y<d.length;y++){g.append("<li>"+d[y]+"</li>")}m.append(g);m.appendTo("body").hide();m.mouseover(function(){a=true}).mouseout(function(){a=false});e("li",g).mouseover(function(){if(!f){e(p,m).removeClass(h);e(this).addClass(h)}}).mousedown(function(){a=true}).click(function(){t(r,this,m,u);a=false});var b=function(){if(m.is(":visible")){return false}e("li",m).removeClass(h);var t=e(r).offset();m.css({top:t.top+r.offsetHeight,left:t.left});m.show();var i=r.value?s(r.value,u):l;var a=l.getHours()*60+l.getMinutes();var f=i.getHours()*60+i.getMinutes()-a;var p=Math.round(f/u.step);var d=o(new Date(0,0,0,0,p*u.step+a,0));d=l<d&&d<=c?d:l;var v=e("li:contains("+n(d,u)+")",m);if(v.length){v.addClass(h);m[0].scrollTop=v[0].offsetTop}return true};e(r).focus(b).click(b);e(r).blur(function(){if(!a){m.hide()}});e(r)["keydown"](function(n){var i;f=true;var s=m[0].scrollTop;switch(n.keyCode){case 38:if(b()){return false}i=e(p,g);var o=i.prev().addClass(h)[0];if(o){i.removeClass(h);if(o.offsetTop<s){m[0].scrollTop=s-o.offsetHeight}}else{i.removeClass(h);o=e("li:last",g).addClass(h)[0];m[0].scrollTop=o.offsetTop-o.offsetHeight}return false;break;case 40:if(b()){return false}i=e(p,g);var a=i.next().addClass(h)[0];if(a){i.removeClass(h);if(a.offsetTop+a.offsetHeight>s+m[0].offsetHeight){m[0].scrollTop=s+a.offsetHeight}}else{i.removeClass(h);a=e("li:first",g).addClass(h)[0];m[0].scrollTop=0}return false;break;case 13:if(m.is(":visible")){var l=e(p,g)[0];t(r,l,m,u)}return false;break;case 27:m.hide();return false;break}return true});e(r).keyup(function(e){f=false});this.getTime=function(){return s(r.value,u)};this.setTime=function(t){r.value=n(i(t,u),u);e(r).change()}};e.fn.timePicker.defaults={step:30,startTime:new Date(0,0,0,0,0,0),endTime:new Date(0,0,0,23,30,0),separator:":",show24Hours:true}})(jQuery)