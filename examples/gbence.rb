class FeedWM
  def initialize
    @file  = __FILE__
  end

  def upgrade
    now_mtime = File.mtime(@file)
    @last_mtime ||= now_mtime
    return true if @last_mtime == now_mtime
    eval(File.read(@file), TOPLEVEL_BINDING)
    @last_mtime = now_mtime
  end

  def convert_data_load(bytes)
    case bytes
    when 0..1000
      ("%4d" % [ bytes ]).gsub(/\s/, '_')
    when 1000..9999
      ("%3.1fk" % [ (bytes.to_f/1000).to_f ]).gsub(/\s/, '_')
    when 10_000..1_000_000
      ("%3dk" % [ (bytes.to_f/1000.to_f).to_i ]).gsub(/\s/, '_')
    when 1_000_000..9_999_999
      ("%3.1fm" % [ bytes.to_f/1_000_000.to_f ]).gsub(/\s/, '_')
    when 10_000_000..1_000_000_000
      ("%3dm" % [ (bytes.to_f/1_000_000.to_f).to_i ]).gsub(/\s/, '_')
    when 1_000_000_000..9_999_999_999
      ("%3.1fg" % [ bytes.to_f/1_000_000_000.to_f ]).gsub(/\s/, '_')
    when 10_000_000_000..1_000_000_000_000
      ("%3dg" % [ (bytes.to_f/1_000_000_000.to_f).to_i ]).gsub(/\s/, '_')
    end
  end

  def generate
    [
      # load
      begin
        one_minute_average = File.open("/proc/loadavg").read.match(/^([0-9.]*)/)[0].to_f
        #"%s%0.2f%s" % [ [[3,4], [1,3], [0,1]].find { |n| n[0] <= one_minute_average }[1].chr, one_minute_average, 1.chr ]
        "%0.2f" % [ one_minute_average ]
      rescue
        "- (#{$!.message})"
      end,
      # memory
      begin
        memory_details = File.open('/proc/meminfo','r').readlines.inject({}) { |h,l| l =~ /^(\w*):\s*(\d*) kB$/ && h.merge($1.to_s => $2.to_i) || h }
        memory_free    = memory_details['MemFree']*1024
        swap_free      = memory_details['SwapFree']*1024
        #"%s %s/%s%s" % [ [[100,4], [500,3], [100000,0]].find { |n| n[0]*1024*1024 >= memory_free }[1].chr, convert_data_load(memory_free), convert_data_load(swap_free), 1.chr ]
        "%s/%s" % [ convert_data_load(memory_free), convert_data_load(swap_free) ]
      rescue
        "- (#{$!.message})"
      end,
      # network load
      begin
        ifs_up   = File.open('/proc/net/route','r').readlines.map { |l| (l =~ /^((eth|ath|wlan|rndis|tap|tun|ppp|enp\ds|wlp\ds)\d)\s*/) ? $1 : nil }.compact.uniq
        ifs_data = File.open('/proc/net/dev','r').readlines.map { |l| (l =~ /(eth\d|ath\d|wlan\d|rndis\d|tap\d|tun\d|ppp\d|enp\ds\d|wlp\ds\d):\s*(\d*)\s*\d*\s*\d*\s*\d*\s*\d*\s*\d*\s*\d*\s*\d*\s*(\d*)/) ? { $1 => [ $2, $3 ] } : nil }.compact.inject({}) { |hash,i| hash.merge(i) }
        @network_load ||= ifs_data || Array.new([])
        diffs = []
        ifs_data.keys.each { |i|
          diffs << "%2s:^%4sv%4s" % [
            i.gsub(/(th|lan|ndis)/, '').gsub(/t(u|a)(n|p)/, '\1').gsub(/(e|w)(n|l)p\ds/, '\1'),
            convert_data_load(ifs_data[i][1].to_i-@network_load[i][1].to_i), convert_data_load(ifs_data[i][0].to_i-@network_load[i][0].to_i) ] if ifs_up.include? i
        }
        diffs.join(' ')
      rescue
        "- (#{$!.message})"
      ensure
        @network_load = ifs_data
      end,
      # battery
      begin
        charging  = File.read("/sys/class/power_supply/BAT0/status").strip
        last      = File.read("/sys/class/power_supply/BAT0/energy_full").strip.to_i
        current   = File.read("/sys/class/power_supply/BAT0/energy_now").strip.to_i
        percent   = current.to_f * 100.0 / last.to_f
        #remaining = (charging =~ /discharging/i) ? (current.to_f*60 / rate.to_f) : ((last.to_f - current.to_f)*60 / rate.to_f)
        #"%s%.2f%% %.2fm" % [ (charging =~ /discharging/i && remaining < 5.0) ? 4.chr : 1.chr, percent, remaining ]
        #"%s%.2f%%" % [ (charging =~ /discharging/i) ? 4.chr : 1.chr, percent ]
        "%.2f%%" % [ percent ]
      rescue
        "- (#{$!.message})"
      end,
#      # temperature
#      begin
#        temperatures = File.open("/proc/acpi/ibm/thermal").read.split(/\s/)
#        fan          = (File.open("/proc/acpi/ibm/fan").read.match(/speed:\s*([0-9]*)/) || [ "-", "-" ])[1]
#        "%sC %s" % [ (temperatures[5] != "25" ? temperatures[4] : temperatures[7]), fan ]
#      rescue
#        "- (#{$!.message})"
#      end,
      # time
      Time.now.strftime("%m-%d %H:%M:%S").to_s,
    ].join(" ")
  end
end

def feedwm
  begin
    @feedwm ||= FeedWM.new
    @feedwm.upgrade && @feedwm.generate
  rescue
    STDERR.puts $!.inspect
  end
end
