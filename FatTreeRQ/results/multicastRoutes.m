clc,clear, close all

servX=[0:2:6 , 10:2:16 , 20:2:26 , 30:2:36] ;
servY=ones(1,16);
scatter(servX,servY, 'MarkerEdgeColor',[0 .05 .05],  'MarkerFaceColor',[0 .07 .07], 'LineWidth',15)
hold on
edgeX=[1:4:6 , 11:4:16 , 21:4:26 , 31:4:36];
edgeY=2*ones(1,8);

scatter(edgeX,edgeY, 'MarkerEdgeColor',[0 .5 .5],  'MarkerFaceColor',[0 .7 .7], 'LineWidth',15)
hold on
aggX = edgeX;
aggY=4*ones(1,8);
scatter(aggX,aggY, 'MarkerEdgeColor',[0 .5 .5],  'MarkerFaceColor',[0 .7 .7], 'LineWidth',15)
hold on

coreX=[3 ,13, 23, 33];
coreY=6*ones(1,4);

set(gca,'XTick',(0:1:40))
scatter(coreX,coreY, 'MarkerEdgeColor',[0 .6 .9],  'MarkerFaceColor',[0 .2 .5], 'LineWidth',15)

axis([-1.5 40 0 7])
 

% box
x1=-1; x2=7;
y1=0.5; y2=6.5;
x = [x1, x2, x2, x1, x1]; y = [y1, y1, y2, y2, y1];
plot(x, y, 'm--', 'LineWidth', 0.3);
hold on;
 
x1=8; x2=17;
x = [x1, x2, x2, x1, x1]; 
plot(x, y, 'm--', 'LineWidth', 0.3);
hold on;

x1=18; x2=27;
x = [x1, x2, x2, x1, x1]; 
plot(x, y, 'm--', 'LineWidth', 0.3);
hold on;

x1=28; x2=37;
x = [x1, x2, x2, x1, x1]; 
plot(x, y, 'm--', 'LineWidth', 0.3);
hold on;




%%
A=[ 0, 0, 0; 0, 0, 1 ; 0, 1, 0; 0, 1, 1; 1, 0, 0 ;  1, 0, 1; 1, 1, 0; 1, 1, 1; 2, 0, 0; 2, 0, 1 ; 2, 1, 0 ; 2, 1, 1 ; 3, 0, 0 ; 3, 0, 1 ; 3, 1, 0 ; 3, 1, 1 ];
nodes = cell(36, 2);

for i=1:16
    server =['FatTree.Pod[' , num2str(A(i,1)) , '].racks[' , num2str(A(i,2)), '].servers[' , num2str(A(i,3)) ,']' ] ;
    nodes{i,1}= server;
   nodes{i,2}= [servX(i), servY(i)];
end

j=1;
for i=1:8
    edge =['FatTree.Pod[' , num2str(A(j,1)) , '].racks[' , num2str(A(j,2)), '].edgeSwitch'] ;
    nodes{i+16,1} = edge;
    nodes{i+16,2}= [edgeX(i), edgeY(i)];
    j=j+2;
end

j=1;
for i=1:8
    agg =['FatTree.Pod[' , num2str(A(j,1)) , '].aggRouters[' , num2str(A(j,2)), ']'] ;
    nodes{i+24,1} = agg;
    nodes{i+24,2}= [aggX(i), aggY(i)];
    j=j+2;
end

j=1;
for i=1:4
    core =['FatTree.CoreRouter[' , num2str(A(j,1)), ']'] ;
    nodes{i+32,1} = core;
    nodes{i+32,2}= [coreX(i), coreY(i)];
%      text(coreX(i)-0.5,coreY(i)+0.4, sprintf( nodes{i+32,1}),'FontSize',11 )
    j=j+4;
end


data = read_mixed_csv('multicastingRoute.csv',',')
data'

tabel= zeros(length(data)-1,2);
for k=1:length(data)
    for i=1:36
        tf = strcmp(data{k},nodes{i,1});
        if tf == 1
            nodes{i,1};
            tabel(k,:)= nodes{i,2}
            break;
        end
    end
end

 
counter =0;
sender = 0;
for i=1:length(tabel)
    if  (tabel(i,1) ==0) && (tabel(i,2) ==0 )
        tabel(i,:)
        counter = 0; 
    end
    if counter~=0
       p2= tabel(i,:);
       dp= p2-p1;
       hold on
       if sender == 0
           arrow(p1,p2,'Length' ,11 , 'EdgeColor','r','FaceColor','b' ,'Width',3)
           sender =10;
       end
       
       if sender~=0
        arrow(p1,p2,'Length' ,11 , 'EdgeColor','b','FaceColor','r')
       end
%       text(p2(1),p2(2), sprintf(data{1,i}),'FontSize',5 )
    end
    if counter == 0
        if  (tabel(i,1)~=0) || (tabel(i,2)~=0) 
            p1 = tabel(i,:);
            
            counter = counter+1;
        end
    end
end


% 
   ax = gca
   ax.Visible = 'off'