namespace eval Test {
	variable ledValue 0
	variable dashboardActive 0
	variable Switch_off 1

	proc toggle { position } {
		set ::Test::ledValue ${position}
		::Test::updateDashboard
	}

	proc claimMaster {} {
		# Scan chain for JTAG Avalon MM Master
		set mp [lindex [get_service_paths master] 1]
		set service_type "master"
		set claim_path [claim_service $service_type $mp mylib]
		toolkit_set_property MasterPathText text $mp
	}

	proc writeSDRAM32 {} {
		set mp [lindex [get_service_paths master] 1]
		set service_type "master"
		set claim_path [claim_service $service_type $mp mylib]

		set addr [toolkit_get_property AddressInput1 text]
		set data [toolkit_get_property DataInput1 text]
		#puts stdout $addr
		#puts stdout $data

		set rd [master_write_32 $claim_path $addr $data]
	}
	proc writeSDRAM {} {
		set mp [lindex [get_service_paths master] 1]
		set service_type "master"
		set claim_path [claim_service $service_type $mp mylib]

		set addr1 [toolkit_get_property AddressInput text]
		set data1 [toolkit_get_property DataInput text]
		#puts stdout $addr
		#puts stdout $data

		set rd [master_write_memory $claim_path $addr1 $data1]
	}
	proc readSDRAM {} {
		set mp [lindex [get_service_paths master] 1]
		set service_type "master"
		set claim_path [claim_service $service_type $mp mylib]

		set read_addr [toolkit_get_property ReadAddressInput text]

		set rd [master_read_32 $claim_path $read_addr 0x01]

		toolkit_set_property ReadData text $rd
	}

	proc blockReadSDRAM {} {
		set mp [lindex [get_service_paths master] 1]
		set service_type "master"
		set claim_path [claim_service $service_type $mp mylib]

		set read_addr [toolkit_get_property ReadAddressInput text]
		set read_blk [toolkit_get_property AddressRangeInput text]

		set rd [master_read_memory $claim_path $read_addr $read_blk]

		toolkit_set_property receiveTextText text $rd
	}

	proc dashBoard {} {
		if { ${::Test::dashboardActive} == 1 } {
		return -code ok "dashboard already active"
		}

		set ::Test::dashboardActive 1
		#
		# top group widget
		#
		toolkit_add topGroup group self
		toolkit_set_property topGroup expandableX false
		toolkit_set_property topGroup expandableY false
		toolkit_set_property topGroup preferredWidth 1250
		toolkit_set_property topGroup preferredHeight 750
		toolkit_set_property topGroup itemsPerRow 1
		toolkit_set_property topGroup title "HPS SDRAM"
		#
		# JTAG Master group widget
		#
		toolkit_add jtagavmmGroup group topGroup
		toolkit_set_property jtagavmmGroup expandableX false
		toolkit_set_property jtagavmmGroup expandableY false
		toolkit_set_property jtagavmmGroup preferredWidth 1000
		toolkit_set_property jtagavmmGroup preferredHeight 100
		toolkit_set_property jtagavmmGroup itemsPerRow 2
		toolkit_set_property jtagavmmGroup title "JTAG Avalon Master"
		#
		# Command group widget
		#
		toolkit_add CommandGroup group topGroup
		toolkit_set_property CommandGroup expandableX false
		toolkit_set_property CommandGroup expandableY false
		toolkit_set_property CommandGroup preferredWidth 1250
		toolkit_set_property CommandGroup preferredHeight 100
		toolkit_set_property CommandGroup itemsPerRow 2
		toolkit_set_property CommandGroup title "Command Functions"
		#
		# Read group widget
		#
		toolkit_add ReadGroup group CommandGroup
		toolkit_set_property ReadGroup expandableX false
		toolkit_set_property ReadGroup expandableY false
		toolkit_set_property ReadGroup itemsPerRow 5
		toolkit_set_property ReadGroup title "Read Command"
		#
		# Write group widget
		#
		toolkit_add WriteGroup group CommandGroup
		toolkit_set_property WriteGroup expandableX false
		toolkit_set_property WriteGroup expandableY false
		toolkit_set_property WriteGroup preferredWidth 625
		toolkit_set_property WriteGroup itemsPerRow 5
		toolkit_set_property WriteGroup title "Write Command"
		#
		# Open master widgets
		#
		toolkit_add OpenMasterButton button jtagavmmGroup
		toolkit_set_property OpenMasterButton enabled true
		toolkit_set_property OpenMasterButton expandableY false
		toolkit_set_property OpenMasterButton expandableY false
		toolkit_set_property OpenMasterButton text "OpenMaster"
		toolkit_set_property OpenMasterButton onClick {::Test::claimMaster}	

		toolkit_add MasterPathText text jtagavmmGroup
		toolkit_set_property MasterPathText expandableX false
		toolkit_set_property MasterPathText expandableY false
		toolkit_set_property MasterPathText preferredWidth 900
		toolkit_set_property MasterPathText preferredHeight 50
		toolkit_set_property MasterPathText editable false
		toolkit_set_property MasterPathText htmlCapable false
		toolkit_set_property MasterPathText text ""
		#
		# Button widgets
		#
		#Read Group Config
		toolkit_add ReadButton button ReadGroup
		toolkit_set_property ReadButton enabled true
		toolkit_set_property ReadButton expandableY false
		toolkit_set_property ReadButton expandableY false
		toolkit_set_property ReadButton text "Read"
		toolkit_set_property ReadButton onClick {::Test::readSDRAM}

		toolkit_add ReadAddressText text ReadGroup
		toolkit_set_property ReadAddressText expandableX false
		toolkit_set_property ReadAddressText expandableY false
		toolkit_set_property ReadAddressText preferredWidth 70
		toolkit_set_property ReadAddressText preferredHeight 100
		toolkit_set_property ReadAddressText editable false
		toolkit_set_property ReadAddressText htmlCapable false
		toolkit_set_property ReadAddressText text " Address"

		toolkit_add ReadAddressInput text ReadGroup
		toolkit_set_property ReadAddressInput expandableX false
		toolkit_set_property ReadAddressInput expandableY false
		toolkit_set_property ReadAddressInput preferredWidth 120
		toolkit_set_property ReadAddressInput preferredHeight 100
		toolkit_set_property ReadAddressInput editable yes
		toolkit_set_property ReadAddressInput htmlCapable false
		toolkit_set_property ReadAddressInput text "0x1ffffff4"

		toolkit_add ReadDataText text ReadGroup
		toolkit_set_property ReadDataText expandableX false
		toolkit_set_property ReadDataText expandableY false
		toolkit_set_property ReadDataText preferredWidth 70
		toolkit_set_property ReadDataText preferredHeight 100
		toolkit_set_property ReadDataText editable false
		toolkit_set_property ReadDataText htmlCapable false
		toolkit_set_property ReadDataText text " Data"

		toolkit_add ReadData text ReadGroup
		toolkit_set_property ReadData expandableX false
		toolkit_set_property ReadData expandableY false
		toolkit_set_property ReadData preferredWidth 150
		toolkit_set_property ReadData preferredHeight 100
		toolkit_set_property ReadData editable false
		toolkit_set_property ReadData htmlCapable false
		toolkit_set_property ReadData text ""

		#Write Group Config
		toolkit_add WriteButton button WriteGroup
		toolkit_set_property WriteButton enabled true
		toolkit_set_property WriteButton expandableY false
		toolkit_set_property WriteButton expandableY false
		toolkit_set_property WriteButton text "Write data"
		toolkit_set_property WriteButton onClick {::Test::writeSDRAM}

		toolkit_add AddressText text WriteGroup
		toolkit_set_property AddressText expandableX false
		toolkit_set_property AddressText expandableY false
		toolkit_set_property AddressText preferredWidth 70
		toolkit_set_property AddressText preferredHeight 100
		toolkit_set_property AddressText editable false
		toolkit_set_property AddressText htmlCapable false
		toolkit_set_property AddressText text " Address"

		toolkit_add AddressInput text WriteGroup
		toolkit_set_property AddressInput expandableX false
		toolkit_set_property AddressInput expandableY false
		toolkit_set_property AddressInput preferredWidth 150
		toolkit_set_property AddressInput preferredHeight 100
		toolkit_set_property AddressInput editable yes
		toolkit_set_property AddressInput htmlCapable false
		toolkit_set_property AddressInput text "0x1ffffff4"

		toolkit_add DataText text WriteGroup
		toolkit_set_property DataText expandableX false
		toolkit_set_property DataText expandableY false
		toolkit_set_property DataText preferredWidth 70
		toolkit_set_property DataText preferredHeight 100
		toolkit_set_property DataText editable false
		toolkit_set_property DataText htmlCapable false
		toolkit_set_property DataText text " Data"

		toolkit_add DataInput text WriteGroup
		toolkit_set_property DataInput expandableX false
		toolkit_set_property DataInput expandableY false
		toolkit_set_property DataInput preferredWidth 100
		toolkit_set_property DataInput preferredHeight 100
		toolkit_set_property DataInput editable yes
		toolkit_set_property DataInput htmlCapable false
		toolkit_set_property DataInput text "0x0"
		
		toolkit_add WriteButton1 button WriteGroup
		toolkit_set_property WriteButton1 enabled true
		toolkit_set_property WriteButton1 expandableY false
		toolkit_set_property WriteButton1 expandableY false
		toolkit_set_property WriteButton1 text "Write address"
		toolkit_set_property WriteButton1 onClick {::Test::writeSDRAM32}

		toolkit_add AddressText1 text WriteGroup
		toolkit_set_property AddressText1 expandableX false
		toolkit_set_property AddressText1 expandableY false
		toolkit_set_property AddressText1 preferredWidth 70
		toolkit_set_property AddressText1 preferredHeight 100
		toolkit_set_property AddressText1 editable false
		toolkit_set_property AddressText1 htmlCapable false
		toolkit_set_property AddressText1 text " Address"

		toolkit_add AddressInput1 text WriteGroup
		toolkit_set_property AddressInput1 expandableX false
		toolkit_set_property AddressInput1 expandableY false
		toolkit_set_property AddressInput1 preferredWidth 150
		toolkit_set_property AddressInput1 preferredHeight 100
		toolkit_set_property AddressInput1 editable yes
		toolkit_set_property AddressInput1 htmlCapable false
		toolkit_set_property AddressInput1 text "0x1fffffe4"

		toolkit_add DataText1 text WriteGroup
		toolkit_set_property DataText1 expandableX false
		toolkit_set_property DataText1 expandableY false
		toolkit_set_property DataText1 preferredWidth 70
		toolkit_set_property DataText1 preferredHeight 100
		toolkit_set_property DataText1 editable false
		toolkit_set_property DataText1 htmlCapable false
		toolkit_set_property DataText1 text " Data"

		toolkit_add DataInput1 text WriteGroup
		toolkit_set_property DataInput1 expandableX false
		toolkit_set_property DataInput1 expandableY false
		toolkit_set_property DataInput1 preferredWidth 100
		toolkit_set_property DataInput1 preferredHeight 100
		toolkit_set_property DataInput1 editable yes
		toolkit_set_property DataInput1 htmlCapable false
		toolkit_set_property DataInput1 text "0x0"

		#
		# sendText widgets
		#
		#toolkit_add sendTextGroup group topGroup
		#toolkit_set_property sendTextGroup expandableX false
		#toolkit_set_property sendTextGroup expandableY false
		#toolkit_set_property sendTextGroup itemsPerRow 1
		#toolkit_set_property sendTextGroup title "Send Data"

		#toolkit_add sendTextText text sendTextGroup
		#toolkit_set_property sendTextText expandableX false
		#toolkit_set_property sendTextText expandableY false
		#toolkit_set_property sendTextText preferredWidth 200
		#toolkit_set_property sendTextText preferredHeight 200
		#toolkit_set_property sendTextText editable true
		#toolkit_set_property sendTextText htmlCapable false
		#toolkit_set_property sendTextText text ""

		#toolkit_add sendTextButton button sendTextGroup
		#toolkit_set_property sendTextButton enabled true
		#toolkit_set_property sendTextButton expandableY false
		#toolkit_set_property sendTextButton expandableY false
		#toolkit_set_property sendTextButton text "Send Now"
		#toolkit_set_property sendTextButton onClick {::Test::sendText}
		#
		# receiveText widgets
		#
		toolkit_add receiveTextGroup group topGroup
		toolkit_set_property receiveTextGroup expandableX false
		toolkit_set_property receiveTextGroup expandableY false
		toolkit_set_property receiveTextGroup itemsPerRow 1
		toolkit_set_property receiveTextGroup title "RAM Contents"
				
		toolkit_add BlockReadButton button receiveTextGroup
		toolkit_set_property BlockReadButton enabled true
		toolkit_set_property BlockReadButton expandableY false
		toolkit_set_property BlockReadButton expandableY false
		toolkit_set_property BlockReadButton text "Block Read"
		toolkit_set_property BlockReadButton onClick {::Test::blockReadSDRAM}

		toolkit_add AddressRangeText text receiveTextGroup
		toolkit_set_property AddressRangeText expandableX false
		toolkit_set_property AddressRangeText expandableY false
		toolkit_set_property AddressRangeText preferredWidth 130
		toolkit_set_property AddressRangeText preferredHeight 60
		toolkit_set_property AddressRangeText editable false
		toolkit_set_property AddressRangeText htmlCapable false
		toolkit_set_property AddressRangeText text " Size of block read"

		toolkit_add AddressRangeInput text receiveTextGroup
		toolkit_set_property AddressRangeInput expandableX false
		toolkit_set_property AddressRangeInput expandableY false
		toolkit_set_property AddressRangeInput preferredWidth 50
		toolkit_set_property AddressRangeInput preferredHeight 60
		toolkit_set_property AddressRangeInput editable yes
		toolkit_set_property AddressRangeInput htmlCapable false
		toolkit_set_property AddressRangeInput text ""
		
		toolkit_add receiveTextText text receiveTextGroup
		toolkit_set_property receiveTextText expandableX false
		toolkit_set_property receiveTextText expandableY false
		toolkit_set_property receiveTextText preferredWidth 310
		toolkit_set_property receiveTextText preferredHeight 300
		toolkit_set_property receiveTextText editable false
		toolkit_set_property receiveTextText htmlCapable false
		toolkit_set_property receiveTextText text ""
		return -code ok
	}


	proc updateDashboard {} {
		if { ${::Test::dashboardActive} > 0 } {
			toolkit_set_property WriteGroup title "Write Command"
			if { [ expr ${::Test::ledValue} & 0x01 & ${::Test::Switch_off} ] } {
			toolkit_set_property led0LED color "green"

		set ::Test::Switch_off 0
			} else {
			toolkit_set_property led0LED color "green_off"
		set ::Test::Switch_off 1
			}
			if { [ expr ${::Test::ledValue} & 0x02 ] } {
			toolkit_set_property led1LED color "green"
			} else {
			toolkit_set_property led1LED color "green_off"
			}
		}
	}
}
::Test::dashBoard
