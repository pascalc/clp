#! /usr/bin/env ruby

command = ARGV[0]
qt = ARGV[1] == "qt"

def build(target, qt=false)
  puts "Building #{target}..."
  if qt
    `g++ -I/usr/local/include -I/Library/Frameworks/QtGui.framework/Headers -c #{target}.cpp`
    `g++ -I/Library/Frameworks/QtGui.framework/Headers -framework gecode -framework QtGui -framework QtCore -o #{target} #{target}.cpp`
  else
    `g++ -O3 -c #{target}.cpp`
    `g++ -framework gecode -o #{target} #{target}.cpp`
  end
end

def clean
  puts "Cleaning..."
  Dir.foreach(Dir.pwd)
    .reject { |f| f =~ /.+\.cpp/ }
    .reject { |f| f =~ /.+\.rb/ }
    .reject { |f| [".", ".."].include? f }
    .each do |f|
      puts "Deleted #{f}"
      File.delete(f)
    end
end

case command
when "clean"
  clean
# when nil
#   puts "Building all .cpp files..."
#   Dir.foreach(Dir.pwd)
#     .select { |f| f =~ /.+\.cpp/ }
#     .each { |f| build(File.basename(f, ".cpp")) }
else
  build(command, qt)
end

puts "Done!"
