t = Thread.new{ sleep 3 }

Signal.trap "SIGINT" do
  t.join
end

puts 'Press ctrl + c now'
t.join
