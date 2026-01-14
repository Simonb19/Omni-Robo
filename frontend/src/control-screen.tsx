import './control-screen.css';
import { Link } from 'react-router-dom';
import IconButton from './components/icon-button.tsx';
import { Settings } from 'lucide-react';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs';
import DriveContent from '@/components/drive-content.tsx';
import GrabContent from '@/components/grab-content.tsx';

function ControlScreen() {
  return (
    <div className="container">
      <Link to={'/debug'}>
        <IconButton icon={Settings} className="icon-button-top-right" />
      </Link>
      <div className="segment-control">
        <Tabs defaultValue="drive">
          <TabsList>
            <TabsTrigger value="drive">Fahren</TabsTrigger>
            <TabsTrigger value="grab">Greifen</TabsTrigger>
            <TabsContent value="drive">
              <DriveContent />
            </TabsContent>
            <TabsContent value="grab">
              <GrabContent />
            </TabsContent>
          </TabsList>
        </Tabs>
      </div>
    </div>
  );
}

export default ControlScreen;
