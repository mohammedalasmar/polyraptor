function plotResults(K,numShortFlows)
 fatTreeDetails(K)
 %saveas(gcf,'fatTreeDetails.png')


 plotThroughput(K,numShortFlows) 
 %saveas(gcf,'plotThroughput.png')

disp( 'press any key to quit Matlab ..')
 pause
 quit
end
